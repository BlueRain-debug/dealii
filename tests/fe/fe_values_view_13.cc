// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2008 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// test the FEValues views and extractor classes. this test is for
// get_function_hessians for vector components and a primitive element

#include <deal.II/base/function.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_q1.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>

#include <deal.II/lac/vector.h>

#include "../tests.h"



template <int dim>
void
test(const Triangulation<dim> &tr, const FiniteElement<dim> &fe)
{
  deallog << "FE=" << fe.get_name() << std::endl;

  DoFHandler<dim> dof(tr);
  dof.distribute_dofs(fe);

  Vector<double> fe_function(dof.n_dofs());
  for (unsigned int i = 0; i < dof.n_dofs(); ++i)
    fe_function(i) = i + 1;

  const QGauss<dim> quadrature(2);
  FEValues<dim>     fe_values(fe,
                          quadrature,
                          update_values | update_gradients | update_hessians);
  fe_values.reinit(dof.begin_active());

  std::vector<Tensor<3, dim>> selected_vector_values(quadrature.size());
  std::vector<std::vector<Tensor<2, dim>>> vector_values(
    quadrature.size(), std::vector<Tensor<2, dim>>(fe.n_components()));

  fe_values.get_function_hessians(fe_function, vector_values);

  for (unsigned int c = 0; c < fe.n_components(); ++c)
    // use a vector extractor if there
    // are sufficiently many components
    // left after the current component
    // 'c'
    if (c + dim <= fe.n_components())
      {
        FEValuesExtractors::Vector vector_components(c);
        fe_values[vector_components].get_function_hessians(
          fe_function, selected_vector_values);
        deallog << "component=" << c << std::endl;

        for (const auto q : fe_values.quadrature_point_indices())
          for (unsigned int d = 0; d < dim; ++d)
            {
              deallog << selected_vector_values[q][d] << std::endl;
              Assert((selected_vector_values[q][d] - vector_values[q][c + d])
                         .norm() <= 1e-12 * selected_vector_values[q][d].norm(),
                     ExcInternalError());
            }
      }
}



template <int dim>
void
test_hyper_sphere()
{
  Triangulation<dim> tr;
  GridGenerator::hyper_ball(tr);

  static const SphericalManifold<dim> boundary;
  tr.set_manifold(0, boundary);

  FESystem<dim> fe(FE_Q<dim>(1),
                   1,
                   FE_Q<dim>(2),
                   2,
                   FE_DGQArbitraryNodes<dim>(QIterated<1>(QTrapezoid<1>(), 3)),
                   dim);
  test(tr, fe);
}


int
main()
{
  initlog();
  deallog << std::setprecision(3);

  test_hyper_sphere<2>();
  test_hyper_sphere<3>();
}
