/* $Id$ */
/* Copyright W. Bangerth, University of Heidelberg, 1998 */


#include <fe/fe.h>
#include <base/quadrature.h>
#include <grid/tria_iterator.h>
#include <grid/dof_accessor.h>
#include <grid/tria_boundary.h>





/*------------------------------- FiniteElementData ----------------------*/

#if deal_II_dimension == 1

FiniteElementData<1>::FiniteElementData () :
		dofs_per_vertex(0),
		dofs_per_line(0),
		dofs_per_face(0),
		total_dofs(0),
		n_transform_functions(0),
		n_components(0)
{
  Assert (false, ExcInternalError());
};



FiniteElementData<1>::FiniteElementData (const unsigned int dofs_per_vertex,
					 const unsigned int dofs_per_line,
					 const unsigned int n_transform_functions,
					 const unsigned int n_components) :
		dofs_per_vertex(dofs_per_vertex),
		dofs_per_line(dofs_per_line),
		dofs_per_face(dofs_per_vertex),
		total_dofs (2*dofs_per_vertex+dofs_per_line),
		n_transform_functions(n_transform_functions),
		n_components(n_components)
{};



FiniteElementData<1>::FiniteElementData (const FiniteElementData<1> &fe_data) :
		dofs_per_vertex(fe_data.dofs_per_vertex),
		dofs_per_line(fe_data.dofs_per_line),
		dofs_per_face(fe_data.dofs_per_face),
		total_dofs (fe_data.total_dofs),
		n_transform_functions(fe_data.n_transform_functions),
		n_components(fe_data.n_components)
{};



bool FiniteElementData<1>::operator== (const FiniteElementData<1> &f) const {
  return ((dofs_per_vertex == f.dofs_per_vertex) &&
	  (dofs_per_line == f.dofs_per_line) &&
	  (total_dofs == f.total_dofs) &&
	  (n_components == f.n_components));
};

#endif



#if deal_II_dimension == 2


FiniteElementData<2>::FiniteElementData () :
		dofs_per_vertex(0),
		dofs_per_line(0),
		dofs_per_quad(0),
		dofs_per_face(0),
		total_dofs(0),
		n_transform_functions(0),
		n_components(0)
{
  Assert (false, ExcInternalError());
};



FiniteElementData<2>::FiniteElementData (const unsigned int dofs_per_vertex,
					 const unsigned int dofs_per_line,
					 const unsigned int dofs_per_quad,
					 const unsigned int n_transform_functions,
					 const unsigned int n_components) :
		dofs_per_vertex(dofs_per_vertex),
		dofs_per_line(dofs_per_line),
		dofs_per_quad(dofs_per_quad),
		dofs_per_face(2*dofs_per_vertex+
			      dofs_per_line),
		total_dofs (4*dofs_per_vertex+
			    4*dofs_per_line+
			    dofs_per_quad),
		n_transform_functions (n_transform_functions),
		n_components(n_components)
{};



FiniteElementData<2>::FiniteElementData (const FiniteElementData<2> &fe_data) :
		dofs_per_vertex(fe_data.dofs_per_vertex),
		dofs_per_line(fe_data.dofs_per_line),
		dofs_per_quad(fe_data.dofs_per_quad),
		dofs_per_face(fe_data.dofs_per_face),
		total_dofs (fe_data.total_dofs),
		n_transform_functions (fe_data.n_transform_functions),
		n_components(fe_data.n_components)
{};



bool FiniteElementData<2>::operator== (const FiniteElementData<2> &f) const {
  return ((dofs_per_vertex == f.dofs_per_vertex) &&
	  (dofs_per_line == f.dofs_per_line) &&
	  (dofs_per_quad == f.dofs_per_quad) &&
	  (total_dofs == f.total_dofs) &&
	  (n_components == f.n_components));
};

#endif



/*------------------------------- FiniteElementBase ----------------------*/


#if deal_II_dimension == 1

template <>
FiniteElementBase<1>::FiniteElementBase (const FiniteElementData<1> &fe_data) :
		FiniteElementData<1> (fe_data)
{
  const unsigned int dim=1;
  for (unsigned int i=0; i<GeometryInfo<dim>::children_per_cell; ++i) 
    {
      restriction[i].reinit (total_dofs, total_dofs);
      prolongation[i].reinit (total_dofs, total_dofs);
    };
  interface_constraints.reinit (1,1);
  interface_constraints(0,0)=1.;
};

#endif


#if deal_II_dimension == 2

template <>
FiniteElementBase<2>::FiniteElementBase (const FiniteElementData<2> &fe_data) :
		FiniteElementData<2> (fe_data)
{
  const unsigned int dim=2;
  for (unsigned int i=0; i<GeometryInfo<dim>::children_per_cell; ++i) 
    {
      restriction[i].reinit (total_dofs, total_dofs);
      prolongation[i].reinit (total_dofs, total_dofs);
    };
  interface_constraints.reinit (dofs_per_vertex+2*dofs_per_line,
				2*dofs_per_vertex+dofs_per_line);
};

#endif



template <int dim>
const dFMatrix &
FiniteElementBase<dim>::restrict (const unsigned int child) const {
  Assert (child<GeometryInfo<dim>::children_per_cell, ExcInvalidIndex(child));
  return restriction[child];
};



template <int dim>
const dFMatrix &
FiniteElementBase<dim>::prolongate (const unsigned int child) const {
  Assert (child<GeometryInfo<dim>::children_per_cell, ExcInvalidIndex(child));
  return prolongation[child];
};



template <int dim>
const dFMatrix &
FiniteElementBase<dim>::constraints () const {
  if (dim==1)
    Assert ((interface_constraints.m()==1) && (interface_constraints.n()==1),
	    ExcWrongInterfaceMatrixSize(interface_constraints.m(),
					interface_constraints.n()));
  
  return interface_constraints;
};



template <int dim>
bool FiniteElementBase<dim>::operator == (const FiniteElementBase<dim> &f) const {
  return ((static_cast<FiniteElementData<dim> >(*this) ==
	   static_cast<FiniteElementData<dim> >(f)) &&
	  (interface_constraints == f.interface_constraints));
};





/*------------------------------- FiniteElement ----------------------*/


template <int dim>
FiniteElement<dim>::FiniteElement (const FiniteElementData<dim> &fe_data) :
		FiniteElementBase<dim> (fe_data) {};



#if deal_II_dimension == 1

// declare this function to be explicitely specialized before first use
// egcs wants this, but gcc2.8.1 produces an internal compiler error, so
// we drop this declaration again for the time being


template <>
void FiniteElement<1>::get_support_points (const DoFHandler<1>::cell_iterator &cell,
					   const Boundary<1> &,
					   vector<Point<1> > &support_points) const;


template <>
void FiniteElement<1>::fill_fe_values (const DoFHandler<1>::cell_iterator &cell,
				       const vector<Point<1> > &unit_points,
				       vector<Tensor<2,1> >    &jacobians,
				       const bool            compute_jacobians,
				       vector<Tensor<3,1> > &jacobians_grad,
				       const bool            compute_jacobians_grad,
				       vector<Point<1> >    &support_points,
				       const bool            compute_support_points,
				       vector<Point<1> >    &q_points,
				       const bool            compute_q_points,
				       const dFMatrix       &,
				       const vector<vector<Tensor<1,1> > > &,
				       const Boundary<1> &boundary) const {
  Assert (jacobians.size() == unit_points.size(),
	  ExcWrongFieldDimension(jacobians.size(), unit_points.size()));
  Assert (q_points.size() == unit_points.size(),
	  ExcWrongFieldDimension(q_points.size(), unit_points.size()));
  Assert (support_points.size() == total_dofs,
	  ExcWrongFieldDimension(support_points.size(), total_dofs));


				   // local mesh width
  const double h=(cell->vertex(1)(0) - cell->vertex(0)(0));

  for (unsigned int i=0; i<q_points.size(); ++i) 
    {
      if (compute_jacobians)
	jacobians[i][0][0] = 1./h;

				       // gradient of jacobian is zero
      if (compute_jacobians_grad)
	jacobians_grad[i] = Tensor<3,1>();
      
      if (compute_q_points)
					 // assume a linear mapping from unit
					 // to real space. overload this
					 // function if you don't like that
	q_points[i] = cell->vertex(0) + h*unit_points[i];
    };

				   // compute support points. The first ones
				   // belong to vertex one, the second ones
				   // to vertex two, all following are
				   // equally spaced along the line
  if (compute_support_points)
    get_support_points (cell, boundary, support_points);
};



template <>
void FiniteElement<1>::fill_fe_face_values (const DoFHandler<1>::cell_iterator &,
					    const unsigned int       ,
					    const vector<Point<0> > &,
					    const vector<Point<1> > &,
					    vector<Tensor<2,1> >    &,
					    const bool               ,
					    vector<Tensor<3,1> >    &,
					    const bool               ,
					    vector<Point<1> >       &,
					    const bool               ,
					    vector<Point<1> >       &,
					    const bool               ,
					    vector<double>          &,
					    const bool              ,
					    vector<Point<1> >       &,
					    const bool,
					    const dFMatrix          &,
					    const vector<vector<Tensor<1,1> > > &,
					    const Boundary<1>       &) const {
  Assert (false, ExcNotImplemented());
};


template <>
void FiniteElement<1>::fill_fe_subface_values (const DoFHandler<1>::cell_iterator &,
					       const unsigned int       ,
					       const unsigned int       ,
					       const vector<Point<0> > &,
					       const vector<Point<1> > &,
					       vector<Tensor<2,1> >    &,
					       const bool               ,
					       vector<Tensor<3,1> >    &,
					       const bool               ,
					       vector<Point<1> >       &,
					       const bool               ,
					       vector<double>          &,
					       const bool               ,
					       vector<Point<1> >       &,
					       const bool,
					       const dFMatrix          &,
					       const vector<vector<Tensor<1,1> > > &,
					       const Boundary<1>       &) const {
  Assert (false, ExcNotImplemented());
};



template <>
void FiniteElement<1>::get_unit_support_points (vector<Point<1> > &support_points) const {
  Assert (support_points.size() == total_dofs,
	  ExcWrongFieldDimension(support_points.size(), total_dofs));
				   // compute support points. The first ones
				   // belong to vertex one, the second ones
				   // to vertex two, all following are
				   // equally spaced along the line
  unsigned int next = 0;
				   // first the dofs in the vertices
  for (unsigned int i=0; i<dofs_per_vertex; ++i)
    support_points[next++] = Point<1>(0.0);
  for (unsigned int i=0; i<dofs_per_vertex; ++i)
    support_points[next++] = Point<1>(1.0);
  
				   // now dofs on line
  for (unsigned int i=0; i<dofs_per_line; ++i) 
    support_points[next++] = Point<1>((i+1.0)/(dofs_per_line+1.0));
};



template <>
void FiniteElement<1>::get_support_points (const DoFHandler<1>::cell_iterator &cell,
					   const Boundary<1> &,
					   vector<Point<1> > &support_points) const {
  Assert (support_points.size() == total_dofs,
	  ExcWrongFieldDimension(support_points.size(), total_dofs));
				   // compute support points. The first ones
				   // belong to vertex one, the second ones
				   // to vertex two, all following are
				   // equally spaced along the line
  unsigned int next = 0;
				   // local mesh width
  const double h=(cell->vertex(1)(0) - cell->vertex(0)(0));
				   // first the dofs in the vertices
  for (unsigned int vertex=0; vertex<2; vertex++) 
    for (unsigned int i=0; i<dofs_per_vertex; ++i)
      support_points[next++] = cell->vertex(vertex);
  
				   // now dofs on line
  for (unsigned int i=0; i<dofs_per_line; ++i) 
    support_points[next++] = cell->vertex(0) +
			    Point<1>((i+1.0)/(dofs_per_line+1.0)*h);
};

#endif



template <int dim>
void FiniteElement<dim>::fill_fe_values (const DoFHandler<dim>::cell_iterator &,
					 const vector<Point<dim> > &,
					 vector<Tensor<2,dim> > &,
					 const bool,
					 vector<Tensor<3,dim> > &,
					 const bool,
					 vector<Point<dim> > &,
					 const bool,
					 vector<Point<dim> > &,
					 const bool,
					 const dFMatrix      &,
					 const vector<vector<Tensor<1,dim> > > &,
					 const Boundary<dim> &) const {
  Assert (false, ExcPureFunctionCalled());
};



template <int dim>
void FiniteElement<dim>::fill_fe_face_values (const DoFHandler<dim>::cell_iterator &cell,
					      const unsigned int           face_no,
					      const vector<Point<dim-1> > &unit_points,
					      const vector<Point<dim> > &global_unit_points,
					      vector<Tensor<2,dim> >    &jacobians,
					      const bool           compute_jacobians,
					      vector<Tensor<3,dim> >    &jacobians_grad,
					      const bool           compute_jacobians_grad,
					      vector<Point<dim> > &support_points,
					      const bool           compute_support_points,
					      vector<Point<dim> > &q_points,
					      const bool           compute_q_points,
					      vector<double>      &face_jacobi_determinants,
					      const bool           compute_face_jacobians,
					      vector<Point<dim> > &normal_vectors,
					      const bool           compute_normal_vectors,
					      const dFMatrix      &shape_values_transform,
					      const vector<vector<Tensor<1,dim> > > &shape_gradients_transform,
					      const Boundary<dim> &boundary) const {
  Assert (jacobians.size() == unit_points.size(),
	  ExcWrongFieldDimension(jacobians.size(), unit_points.size()));
  Assert (q_points.size() == unit_points.size(),
	  ExcWrongFieldDimension(q_points.size(), unit_points.size()));
  Assert (global_unit_points.size() == unit_points.size(),
	  ExcWrongFieldDimension(global_unit_points.size(), unit_points.size()));
  Assert (support_points.size() == dofs_per_face,
	  ExcWrongFieldDimension(support_points.size(), dofs_per_face));
  
  vector<Point<dim> > dummy(total_dofs);
  fill_fe_values (cell, global_unit_points,
		  jacobians, compute_jacobians,
		  jacobians_grad, compute_jacobians_grad,
		  dummy, false,
		  q_points, compute_q_points,
		  shape_values_transform, shape_gradients_transform,
		  boundary);
  
  if (compute_support_points)
    get_face_support_points (cell->face(face_no), boundary, support_points);

  if (compute_face_jacobians)
    get_face_jacobians (cell->face(face_no), boundary,
			unit_points, face_jacobi_determinants);

  if (compute_normal_vectors)
    get_normal_vectors (cell, face_no, boundary,
			unit_points, normal_vectors);
};




template <int dim>
void FiniteElement<dim>::fill_fe_subface_values (const DoFHandler<dim>::cell_iterator &cell,
						 const unsigned int           face_no,
						 const unsigned int           subface_no,
						 const vector<Point<dim-1> > &unit_points,
						 const vector<Point<dim> > &global_unit_points,
						 vector<Tensor<2,dim> >    &jacobians,
						 const bool           compute_jacobians,
						 vector<Tensor<3,dim> >    &jacobians_grad,
						 const bool           compute_jacobians_grad,
						 vector<Point<dim> > &q_points,
						 const bool           compute_q_points,
						 vector<double>      &face_jacobi_determinants,
						 const bool           compute_face_jacobians,
						 vector<Point<dim> > &normal_vectors,
						 const bool           compute_normal_vectors,
						 const dFMatrix      &shape_values_transform,
						 const vector<vector<Tensor<1,dim> > > &shape_gradients_transform,
						 const Boundary<dim> &boundary) const {
  Assert (jacobians.size() == unit_points.size(),
	  ExcWrongFieldDimension(jacobians.size(), unit_points.size()));
  Assert (q_points.size() == unit_points.size(),
	  ExcWrongFieldDimension(q_points.size(), unit_points.size()));
  Assert (global_unit_points.size() == unit_points.size(),
	  ExcWrongFieldDimension(global_unit_points.size(), unit_points.size()));

  vector<Point<dim> > dummy(total_dofs);
  fill_fe_values (cell, global_unit_points,
		  jacobians, compute_jacobians,
		  jacobians_grad, compute_jacobians_grad,
		  dummy, false,
		  q_points, compute_q_points,
		  shape_values_transform, shape_gradients_transform,
		  boundary);
  
  if (compute_face_jacobians)
    get_subface_jacobians (cell->face(face_no), subface_no,
			   unit_points, face_jacobi_determinants);

  if (compute_normal_vectors)
    get_normal_vectors (cell, face_no, subface_no,
			unit_points, normal_vectors);
};



template <int dim>
void FiniteElement<dim>::get_unit_support_points (vector<Point<dim> > &) const {
  Assert (false, ExcPureFunctionCalled());
};



template <int dim>
void FiniteElement<dim>::get_support_points (const DoFHandler<dim>::cell_iterator &,
					    const Boundary<dim> &,
					    vector<Point<dim> > &) const {
  Assert (false, ExcPureFunctionCalled());
};





/*------------------------------- Explicit Instantiations -------------*/

template class FiniteElementBase<deal_II_dimension>;
template class FiniteElement<deal_II_dimension>;



