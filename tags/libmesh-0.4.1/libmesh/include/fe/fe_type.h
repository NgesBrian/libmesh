// $Id: fe_type.h,v 1.1 2003-11-05 22:26:43 benkirk Exp $

// The Next Great Finite Element Library.
// Copyright (C) 2002-2003  Benjamin S. Kirk, John W. Peterson
  
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
  
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



#ifndef __fe_type_h__
#define __fe_type_h__

// C++ includes

// Local includes
#include "libmesh_config.h"
#include "enum_order.h"
#include "enum_fe_family.h"
#include "enum_inf_map_type.h"


/**
 * class FEType hides (possibly multiple) FEFamily and approximation
 * orders, thereby enabling specialized finite element families.
 */
class FEType
{
public:

#ifndef ENABLE_INFINITE_ELEMENTS
  
  /**
   * Constructor.  Optionally takes the approximation \p Order
   * and the finite element family \p FEFamily
   */
  FEType(const Order    o = FIRST,
	 const FEFamily f = LAGRANGE) :
    order(o),
    family(f)
  {}


  //TODO:[BSK] Could these data types all be const?  
  /**
   * The approximation order of the element.  
   */
  Order order;

  /**
   * The type of finite element.  Valid types are \p LAGRANGE,
   * \p HIERARCHIC, etc...
   */
  FEFamily family;
    
#else
  
  /**
   * Constructor.  Optionally takes the approximation \p Order
   * and the finite element family \p FEFamily.  Note that for
   * non-infinite elements the \p order and \p base order are the
   * same, as with the \p family and \p base_family.  It must be
   * so, otherwise what we switch on would change when infinite
   * elements are not compiled in.
   */
  FEType(const Order      o  = FIRST,
	 const FEFamily   f  = LAGRANGE,
	 const Order      ro = THIRD,
	 const FEFamily   rf = JACOBI_20_00,
	 const InfMapType im = CARTESIAN) :
    order(o),
    radial_order(ro),
    family(f),
    radial_family(rf),
    inf_map(im)
  {}


  /**
   * The approximation order in radial direction of the infinite element.  
   */
  Order order;

  /**
   * The approximation order in the base of the infinite element.
   */
  Order radial_order;

  /**
   * The type of approximation in radial direction.  Valid types are 
   * \p JACOBI_20_00, \p JACOBI_30_00, etc...
   */
  FEFamily family;

  /**
   * For InfFE, \p family contains the radial shape family, while
   * \p base_family contains the approximation type in circumferential
   * direction.  Valid types are \p LAGRANGE, \p HIERARCHIC, etc...
   */
  FEFamily radial_family;

  /**
   * The coordinate mapping type of the infinite element.
   * When the infinite elements are defined over a surface with
   * a separable coordinate system (sphere, spheroid, ellipsoid),
   * the infinite elements may take advantage of this fact.
   */
  InfMapType inf_map;
  
#endif // ifndef ENABLE_INFINITE_ELEMENTS

  /**
   * @returns the default quadrature order for this \p FEType.  The
   * default quadrature order is calculated assuming a polynomial of
   * degree \p order and is based on integrating the mass matrix for
   * such an element exactly.
   */
  Order default_quadrature_order () const;

  
private:  
  
};



//-------------------------------------------------------------------
// FEType inline methods
inline
Order FEType::default_quadrature_order () const
{
  return static_cast<Order>(2*static_cast<unsigned int>(order) + 1);
}


#endif // #ifndef __fe_type_h__



