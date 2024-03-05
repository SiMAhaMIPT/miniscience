#include <dolfin.h>
#include "Poisson.h"

using namespace dolfin;

// Border conditions
class Border_cond : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    //values[0] = x[0]+x[1]-x[2];
    values[0] = (x[0]+x[1])*x[2];
    
  }
};

// Source term (right-hand side)
class Source : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    double dx = x[0] - 0.5;
    double dy = x[1] - 0.5;
    double dz = x[2] - 0.5;
    values[0] = 10*exp(-(dx*dx + dy*dy + dz*dz) / 0.02);
  }
};

// Normal derivative (Neumann boundary condition)
class dUdN : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    values[0] = 1000*sin(5*x[0]);
  }
};

// Sub domain for Dirichlet boundary condition
class DirichletBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    return x[0] < DOLFIN_EPS or x[0] > 1.0 - DOLFIN_EPS or
           x[1] < DOLFIN_EPS or x[1] > 1.0 - DOLFIN_EPS or
           x[2] < DOLFIN_EPS or x[2] > 1.0 - DOLFIN_EPS;
  }
};

int main()
{
  // Create mesh and function space
  auto mesh = std::make_shared<Mesh>(
    
    //UnitTetrahedronMesh::create());
    UnitCubeMesh::create({{32, 32, 32}}, CellType::Type::tetrahedron));
  auto V = std::make_shared<Poisson::FunctionSpace>(mesh);

  // Define boundary condition
  auto u0 = std::make_shared<Border_cond>();
  auto boundary = std::make_shared<DirichletBoundary>();
  DirichletBC bc(V, u0, boundary);

  // Define variational forms
  Poisson::BilinearForm a(V, V);
  Poisson::LinearForm L(V);
  auto f = std::make_shared<Source>();
  L.f = f;

  // Compute solution
  Function u(V);
  solve(a == L, u, bc);

  // Save solution in VTK format
  File file("poisson.pvd");
  file << u;

  return 0;
}
