/*
 *  AccumulatorStats.h
 *  Apto
 *
 *  Created by David on 2/16/12.
 *  Copyright 2012 David Michael Bryson. All rights reserved.
 *  http://programerror.com/software/apto
 *
 *  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 *  following conditions are met:
 *  
 *  1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the
 *      following disclaimer.
 *  2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *      following disclaimer in the documentation and/or other materials provided with the distribution.
 *  3.  Neither the name of David Michael Bryson, nor the names of contributors may be used to endorse or promote
 *      products derived from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY DAVID MICHAEL BRYSON AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL DAVID MICHAEL BRYSON OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AptoStatAccumulatorStats_h
#define AptoStatAccumulatorStats_h

#include "apto/core/TypeList.h"

#include <cmath>
#include <limits>


namespace Apto {
  namespace Stat {
    namespace AccumulatorStats {

      
      template <class AccTypes, class TList, template <class _AccTypes, class StatType, class Base> class Unit,
                class Root = EmptyType>
      class DependsOn;
      
      template <class AccTypes, class T1, class T2, template <class, class, class> class Unit, class Root>
      class DependsOn<AccTypes, TypeList<T1, T2>, Unit, Root>
        : public TypeSelect<IsSubclassOf<T1, DependsOn<AccTypes, T2, Unit, Root> >::Result,
                            DependsOn<AccTypes, T2, Unit, Root>,
                            Unit<AccTypes, T1, DependsOn<AccTypes, T2, Unit, Root> > >::Result
      {
      public:
        typedef typename TypeSelect<IsSubclassOf<T1, DependsOn<AccTypes, T2, Unit, Root> >::Result,
                                    typename DependsOn<AccTypes, T2, Unit, Root>::SubClass,
                                    Unit<AccTypes, T1, DependsOn<AccTypes, T2, Unit, Root> > >::Result SubClass;
      };
            
      template <class AccTypes, class T, template <class, class, class> class Unit, class Root>
      class DependsOn<AccTypes, TypeList<T, NullType>, Unit, Root> : public Unit<AccTypes, T, Root>
      {
      protected:
        typedef Unit<AccTypes, T, Root> SubClass;
      };
      
      template <class AccTypes, template <class, class, class> class Unit, class Root>
      class DependsOn<AccTypes, NullType, Unit, Root> : public Root
      {
      protected:
        typedef NullType SubClass;
      };
      
      
      
      class Count { ; };
      class Sum { ; };
      class Mean { ; };
      
      
      template <class AccTypes, class F, class Base>
      class StatImpl : public Base
      {
      protected:
        typedef Base SubClass;
        typedef typename AccTypes::ValueType ValueType;
        
        inline void clear() { ; }
        inline void addValue(ValueType value) { (void)value; }
      };
      
      
      template <class AccTypes, class Base> class StatImpl<AccTypes, Count, Base> : public Base
      {
      private:
        std::size_t m_n;
        
      protected:
        typedef Base SubClass;
        typedef typename AccTypes::ValueType ValueType;
        
        inline void clear() { SubClass::clear(); m_n = 0; }
        inline void addValue(ValueType value) { SubClass::addValue(value); m_n++; }
        
      public:
        std::size_t Count() const { return m_n; }
      };
      
      
      template <class AccTypes, typename Base> class StatImpl<AccTypes, Sum, Base> : public Base
      {
      protected:
        typedef Base SubClass;
        typedef typename Base::ValueType ValueType;
        
      private:
        ValueType m_s;
        
      protected:
        inline void clear() { SubClass::clear(); m_s = 0; }
        inline void addValue(ValueType value) { SubClass::addValue(value); m_s += value; }
        
      public:
        ValueType Sum() const { return m_s; }
      };
      
      
      template <class AccTypes, typename Base> class StatImpl<AccTypes, Mean, Base>
        : public DependsOn<AccTypes, typename TL::Create<Count, Sum, Base>::Type, Apto::Stat::AccumulatorStats::StatImpl>
      {
      protected:
        typedef typename DependsOn<AccTypes,
                                   typename TL::Create<Count, Sum, Base>::Type,
                                   Apto::Stat::AccumulatorStats::StatImpl>::SubClass SubClass;
        typedef typename AccTypes::ValueType ValueType;
        typedef typename AccTypes::FloatType FloatType;
        
        inline void clear() { SubClass::clear(); }
        inline void addValue(ValueType value) { SubClass::addValue(value); }
        
      public:
        inline FloatType Mean() { return (this->Count() > 0) ? (static_cast<FloatType>(this->Sum()) / this->Count()) :
                                                              std::numeric_limits<FloatType>::quiet_NaN(); }
      };
    };
  };
};

#endif