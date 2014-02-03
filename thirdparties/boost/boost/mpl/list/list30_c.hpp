
#ifndef BOOST_MPL_LIST_LIST30_C_HPP_INCLUDED
#define BOOST_MPL_LIST_LIST30_C_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: list30_c.hpp 49239 2008-10-10 09:10:26Z agurtovoy $
// $Date: 2008-10-10 18:10:26 +0900 (금, 10 10 2008) $
// $Revision: 49239 $

#if !defined(BOOST_MPL_PREPROCESSING_MODE)
#   include <boost/mpl/list/list20_c.hpp>
#endif

#include <boost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
 && !defined(BOOST_MPL_PREPROCESSING_MODE)

#   define BOOST_MPL_PREPROCESSED_HEADER list30_c.hpp
#   include <boost/mpl/list/aux_/include_preprocessed.hpp>

#else

#   include <boost/preprocessor/iterate.hpp>

namespace boost { namespace mpl {

#   define BOOST_PP_ITERATION_PARAMS_1 \
    (3,(21, 30, <boost/mpl/list/aux_/numbered_c.hpp>))
#   include BOOST_PP_ITERATE()

}}

#endif // BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#endif // BOOST_MPL_LIST_LIST30_C_HPP_INCLUDED