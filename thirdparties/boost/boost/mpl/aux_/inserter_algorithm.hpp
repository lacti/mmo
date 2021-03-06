
#ifndef BOOST_MPL_AUX_INSERTER_ALGORITHM_HPP_INCLUDED
#define BOOST_MPL_AUX_INSERTER_ALGORITHM_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: inserter_algorithm.hpp 86245 2013-10-11 23:17:48Z skelly $
// $Date: 2013-10-12 08:17:48 +0900 (토, 12 10 2013) $
// $Revision: 86245 $

#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/front_inserter.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/front_inserter.hpp>
#include <boost/mpl/clear.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/aux_/na.hpp>
#include <boost/mpl/aux_/common_name_wknd.hpp>
#include <boost/mpl/aux_/na_spec.hpp>
#include <boost/mpl/aux_/preprocessor/params.hpp>
#include <boost/mpl/aux_/preprocessor/default_params.hpp>
#include <boost/mpl/aux_/config/ctps.hpp>

#include <boost/preprocessor/arithmetic/dec.hpp>


#   define BOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(arity, name) \
BOOST_MPL_AUX_COMMON_NAME_WKND(name) \
template< \
      BOOST_MPL_PP_DEFAULT_PARAMS(arity, typename P, na) \
    > \
struct name \
    : aux::name##_impl<BOOST_MPL_PP_PARAMS(arity, P)> \
{ \
}; \
\
template< \
      BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), typename P) \
    > \
struct name< BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), P),na > \
    : if_< has_push_back< typename clear<P1>::type> \
        , aux::name##_impl< \
              BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), P) \
            , back_inserter< typename clear<P1>::type > \
            > \
        , aux::reverse_##name##_impl< \
              BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), P) \
            , front_inserter< typename clear<P1>::type > \
            > \
        >::type \
{ \
}; \
\
template< \
      BOOST_MPL_PP_DEFAULT_PARAMS(arity, typename P, na) \
    > \
struct reverse_##name \
    : aux::reverse_##name##_impl<BOOST_MPL_PP_PARAMS(arity, P)> \
{ \
}; \
\
template< \
      BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), typename P) \
    > \
struct reverse_##name< BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), P),na > \
    : if_< has_push_back<P1> \
        , aux::reverse_##name##_impl< \
              BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), P) \
            , back_inserter< typename clear<P1>::type > \
            > \
        , aux::name##_impl< \
              BOOST_MPL_PP_PARAMS(BOOST_PP_DEC(arity), P) \
            , front_inserter< typename clear<P1>::type > \
            > \
        >::type \
{ \
}; \
BOOST_MPL_AUX_NA_SPEC(arity, name) \
BOOST_MPL_AUX_NA_SPEC(arity, reverse_##name) \
/**/


#endif // BOOST_MPL_AUX_INSERTER_ALGORITHM_HPP_INCLUDED
