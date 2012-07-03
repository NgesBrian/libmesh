#ifndef __assembly_h__
#define __assembly_h__

#if defined(LIBMESH_HAVE_SLEPC) && defined(LIBMESH_HAVE_GLPK)

#include "sparse_matrix.h"
#include "numeric_vector.h"
#include "dense_matrix.h"
#include "dense_vector.h"
#include "fe.h"
#include "fe_interface.h"
#include "fe_base.h"
#include "elem_assembly.h"
#include "quadrature_gauss.h"

// rbOOmit includes
#include "rb_theta.h"
#include "rb_assembly_expansion.h"

// Bring in bits from the libMesh namespace.
// Just the bits we're using, since this is a header.
using libMesh::ElemAssembly;
using libMesh::FEInterface;
using libMesh::FEMContext;
using libMesh::Number;
using libMesh::Point;
using libMesh::RBTheta;
using libMesh::Real;
using libMesh::RealGradient;

// Functors for the parameter-dependent part of the affine decomposition of the PDE
// The RHS and outputs just require a constant value of 1, so use a default RBTheta object there
struct ThetaA0 : RBTheta { virtual Number evaluate(const RBParameters& mu) { return mu.get_value("mu_0"); } };
struct ThetaA1 : RBTheta { virtual Number evaluate(const RBParameters& mu) { return mu.get_value("mu_1"); } };
struct ThetaA2 : RBTheta { virtual Number evaluate(const RBParameters& mu) { return mu.get_value("mu_2"); } };

struct B : ElemAssembly
{
  // Assemble the H1 inner product. This will be used as the inner product
  // for this problem.
  virtual void interior_assembly(FEMContext &c)
  {
    const unsigned int u_var = 0;

    const std::vector<Real> &JxW =
      c.element_fe_var[u_var]->get_JxW();

    const std::vector<std::vector<Real> >& phi =
      c.element_fe_var[u_var]->get_phi();

    // The velocity shape function gradients at interior
    // quadrature points.
    const std::vector<std::vector<RealGradient> >& dphi =
      c.element_fe_var[u_var]->get_dphi();

    // The number of local degrees of freedom in each variable
    const unsigned int n_u_dofs = c.dof_indices_var[u_var].size();

    // Now we will build the affine operator
    unsigned int n_qpoints = (c.get_element_qrule())->n_points();

    for (unsigned int qp=0; qp != n_qpoints; qp++)
      for (unsigned int i=0; i != n_u_dofs; i++)
        for (unsigned int j=0; j != n_u_dofs; j++)
          c.elem_jacobian(i,j) += JxW[qp] * (phi[j][qp]*phi[i][qp] + dphi[j][qp]*dphi[i][qp]);
  }
};


struct A0 : ElemAssembly
{
  // Assemble the Laplacian operator
  virtual void interior_assembly(FEMContext &c)
  {
    const unsigned int u_var = 0;

    const std::vector<Real> &JxW =
      c.element_fe_var[u_var]->get_JxW();

    // The velocity shape function gradients at interior
    // quadrature points.
    const std::vector<std::vector<RealGradient> >& dphi =
      c.element_fe_var[u_var]->get_dphi();

    // The number of local degrees of freedom in each variable
    const unsigned int n_u_dofs = c.dof_indices_var[u_var].size();

    // Now we will build the affine operator
    unsigned int n_qpoints = (c.get_element_qrule())->n_points();

    Real min_x=0.,max_x=0.5;

    Point centroid = c.elem->centroid();
    if( (min_x <= centroid(0)) && (centroid(0) < max_x) )
    for (unsigned int qp=0; qp != n_qpoints; qp++)
      for (unsigned int i=0; i != n_u_dofs; i++)
        for (unsigned int j=0; j != n_u_dofs; j++)
          c.elem_jacobian(i,j) += JxW[qp] * dphi[j][qp]*dphi[i][qp];
  }
};

struct A1 : ElemAssembly
{
  // Assemble the Laplacian operator
  virtual void interior_assembly(FEMContext &c)
  {
    const unsigned int u_var = 0;

    const std::vector<Real> &JxW =
      c.element_fe_var[u_var]->get_JxW();

    // The velocity shape function gradients at interior
    // quadrature points.
    const std::vector<std::vector<RealGradient> >& dphi =
      c.element_fe_var[u_var]->get_dphi();

    // The number of local degrees of freedom in each variable
    const unsigned int n_u_dofs = c.dof_indices_var[u_var].size();

    // Now we will build the affine operator
    unsigned int n_qpoints = (c.get_element_qrule())->n_points();

    Real min_x=0.5, max_x=1.;

    Point centroid = c.elem->centroid();
    if( (min_x <= centroid(0)) && (centroid(0) <= max_x) )
    for (unsigned int qp=0; qp != n_qpoints; qp++)
      for (unsigned int i=0; i != n_u_dofs; i++)
        for (unsigned int j=0; j != n_u_dofs; j++)
          c.elem_jacobian(i,j) += JxW[qp] * dphi[j][qp]*dphi[i][qp];
  }
};

struct A2 : ElemAssembly
{
  // Convection in the x-direction
  virtual void interior_assembly(FEMContext &c)
  {
    const unsigned int u_var = 0;

    const std::vector<Real> &JxW =
      c.element_fe_var[u_var]->get_JxW();

    const std::vector<std::vector<Real> >& phi =
      c.element_fe_var[u_var]->get_phi();

    const std::vector<std::vector<RealGradient> >& dphi =
      c.element_fe_var[u_var]->get_dphi();

    // The number of local degrees of freedom in each variable
    const unsigned int n_u_dofs = c.dof_indices_var[u_var].size();

    // Now we will build the affine operator
    unsigned int n_qpoints = (c.get_element_qrule())->n_points();

    for (unsigned int qp=0; qp != n_qpoints; qp++)
      for (unsigned int i=0; i != n_u_dofs; i++)
        for (unsigned int j=0; j != n_u_dofs; j++)
          c.elem_jacobian(i,j) -= JxW[qp] *dphi[i][qp](0)*phi[j][qp];
  }
};


struct F0 : ElemAssembly
{
  // Source term, 1 throughout the domain
  virtual void interior_assembly(FEMContext &c)
  {
    const unsigned int u_var = 0;

    const std::vector<Real> &JxW =
      c.element_fe_var[u_var]->get_JxW();

    const std::vector<std::vector<Real> >& phi =
      c.element_fe_var[u_var]->get_phi();

    // The number of local degrees of freedom in each variable
    const unsigned int n_u_dofs = c.dof_indices_var[u_var].size();

    // Now we will build the affine operator
    unsigned int n_qpoints = (c.get_element_qrule())->n_points();

    for (unsigned int qp=0; qp != n_qpoints; qp++)
      for (unsigned int i=0; i != n_u_dofs; i++)
        c.elem_residual(i) += JxW[qp] * ( 1.*phi[i][qp] );
  }
};

struct OutputAssembly : ElemAssembly
{
  OutputAssembly(Real min_x_in, Real max_x_in,
                 Real min_y_in, Real max_y_in)
                :
                min_x(min_x_in),
                max_x(max_x_in),
                min_y(min_y_in),
                max_y(max_y_in)
  {}

  // Output: Average value over the region [min_x,max_x]x[min_y,max_y]
  virtual void interior_assembly(FEMContext &c)
  {
    const unsigned int u_var = 0;

    const std::vector<Real> &JxW =
      c.element_fe_var[u_var]->get_JxW();

    const std::vector<std::vector<Real> >& phi =
      c.element_fe_var[u_var]->get_phi();

    // The number of local degrees of freedom in each variable
    const unsigned int n_u_dofs = c.dof_indices_var[u_var].size();

    // Now we will build the affine operator
    unsigned int n_qpoints = (c.get_element_qrule())->n_points();
    
    Real output_area = (max_x-min_x) * (max_y-min_y);

    Point centroid = c.elem->centroid();
    if( (min_x <= centroid(0)) && (centroid(0) <= max_x) &&
        (min_y <= centroid(1)) && (centroid(1) <= max_y) )
      for (unsigned int qp=0; qp != n_qpoints; qp++)
        for (unsigned int i=0; i != n_u_dofs; i++)
          c.elem_residual(i) += JxW[qp] * ( 1.*phi[i][qp] ) / output_area;
  }
  
  // Member variables that define the output region in 2D
  Real min_x, max_x, min_y, max_y;
};

// Define an RBThetaExpansion class for this PDE
struct Ex02RBThetaExpansion : RBThetaExpansion
{

  /**
   * Constructor.
   */
  Ex02RBThetaExpansion()
  {
    // set up the RBThetaExpansion object
    attach_theta_q_a(&theta_a_0);   // Attach the lhs theta
    attach_theta_q_a(&theta_a_1);
    attach_theta_q_a(&theta_a_2);

    attach_theta_q_f(&rb_theta);    // Attach the rhs theta

    attach_output_theta(&rb_theta); // Attach output 0 theta
    attach_output_theta(&rb_theta); // Attach output 1 theta
    attach_output_theta(&rb_theta); // Attach output 2 theta
    attach_output_theta(&rb_theta); // Attach output 3 theta
  }

  // The RBTheta member variables
  ThetaA0 theta_a_0;
  ThetaA1 theta_a_1;
  ThetaA2 theta_a_2;
  RBTheta rb_theta; // Default RBTheta object, just returns 1.
};

// Define an RBAssemblyExpansion class for this PDE
struct Ex02RBAssemblyExpansion : RBAssemblyExpansion
{

  /**
   * Constructor.
   */
  Ex02RBAssemblyExpansion()
    :
    L0(0.72,0.88,0.72,0.88), // We make sure these output regions conform to the mesh
    L1(0.12,0.28,0.72,0.88),
    L2(0.12,0.28,0.12,0.28),
    L3(0.72,0.88,0.12,0.28)
  {
    // And set up the RBAssemblyExpansion object
    attach_A_q_assembly(&A0_assembly); // Attach the lhs assembly
    attach_A_q_assembly(&A1_assembly);
    attach_A_q_assembly(&A2_assembly);
    
    attach_F_q_assembly(&F0_assembly); // Attach the rhs assembly
    
    attach_output_assembly(&L0);       // Attach output 0 assembly
    attach_output_assembly(&L1);       // Attach output 1 assembly
    attach_output_assembly(&L2);       // Attach output 2 assembly
    attach_output_assembly(&L3);       // Attach output 3 assembly
  }

  // The ElemAssembly objects
  B B_assembly;
  A0 A0_assembly;
  A1 A1_assembly;
  A2 A2_assembly;
  F0 F0_assembly;
  OutputAssembly L0;
  OutputAssembly L1;
  OutputAssembly L2;
  OutputAssembly L3;
};

#endif // LIBMESH_HAVE_SLEPC && LIBMESH_HAVE_GLPK

#endif
