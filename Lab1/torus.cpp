#include <set>
#include <gmsh.h>

int main(int argc, char **argv){
    gmsh::initialize();

    gmsh::model::add("torus");

    float R = 4; //большой радиус
    float r = 2; //малый радиус
    float delta = 0.5; //толщина

    double lc = 1e-1;
    gmsh::model::geo::addPoint(0, 0, 0, lc, 1); //центр

    //основными точками я называю точки, лежащие на деаметре, соответственно, с помощью которых можно построить окружность

    //основные точки внешней окружности
    gmsh::model::geo::addPoint(0, -R, 0, lc, 2);
    gmsh::model::geo::addPoint(0, R, 0, lc, 3);
    //основные точки внутреней окружности
    gmsh::model::geo::addPoint(0, -r, 0, lc, 4);
    gmsh::model::geo::addPoint(0, r, 0, lc, 5);
    //центральная окружность (основные точки) 
    gmsh::model::geo::addPoint(0, -(R+r)/2, 0, lc, 6);
    gmsh::model::geo::addPoint(0, (R+r)/2, 0, lc, 7);
    //объем max
    gmsh::model::geo::addPoint(0, -R+delta, 0, lc, 8);
    gmsh::model::geo::addPoint(0, R-delta, 0, lc, 9);
    //объем min
    gmsh::model::geo::addPoint(0, -r-delta, 0, lc, 10);
    gmsh::model::geo::addPoint(0, r+delta, 0, lc, 11);

    //внутренняя и внешняя окружности
    gmsh::model::geo::addCircleArc(2, 1, 3, 1);
    gmsh::model::geo::addCircleArc(3, 1, 2, 2);
    gmsh::model::geo::addCircleArc(4, 1, 5, 3);
    gmsh::model::geo::addCircleArc(5, 1, 4, 4);

    //соответствующи окружности в xz
    gmsh::model::geo::addCircleArc(4, 6, 2, 5, 1, 0, 0);
    gmsh::model::geo::addCircleArc(2, 6, 4, 6, 1, 0, 0);
    gmsh::model::geo::addCircleArc(3, 7, 5, 7, 1, 0, 0);
    gmsh::model::geo::addCircleArc(5, 7, 3, 8, 1, 0, 0);

    //вложение
    gmsh::model::geo::addCircleArc(8, 1, 9, 9);
    gmsh::model::geo::addCircleArc(9, 1, 8, 10);
    gmsh::model::geo::addCircleArc(10, 1, 11, 11);
    gmsh::model::geo::addCircleArc(11, 1, 10, 12);
    gmsh::model::geo::addCircleArc(9, 7, 11, 13, 1, 0, 0);
    gmsh::model::geo::addCircleArc(11, 7, 9, 14, 1, 0, 0);
    gmsh::model::geo::addCircleArc(10, 6, 8, 15, 1, 0, 0);
    gmsh::model::geo::addCircleArc(8, 6, 10, 16, 1, 0, 0); 

    //cоздание контуров поверхностей и поверхностей для тора
    gmsh::model::geo::addCurveLoop({1, 7, -3, 5}, 1);
    gmsh::model::geo::addSurfaceFilling({1}, 1);
    gmsh::model::geo::addCurveLoop({8, -1, 6, 3}, 2);
    gmsh::model::geo::addSurfaceFilling({2}, 2);
    gmsh::model::geo::addCurveLoop({7, 4, 5, -2}, 3);
    gmsh::model::geo::addSurfaceFilling({3}, 3);
    gmsh::model::geo::addCurveLoop({8, 2, 6, -4}, 4);
    gmsh::model::geo::addSurfaceFilling({4}, 4);

    //создание контуров поверхностей и поверхностей для вложения
    gmsh::model::geo::addCurveLoop({15, -10, 13, 12}, 5);
    gmsh::model::geo::addSurfaceFilling({5}, 5);
    gmsh::model::geo::addCurveLoop({16, -12, 14, 10}, 6);
    gmsh::model::geo::addSurfaceFilling({6}, 6);
    gmsh::model::geo::addCurveLoop({15, 9, 13, -11}, 7);
    gmsh::model::geo::addSurfaceFilling({7}, 7);
    gmsh::model::geo::addCurveLoop({16, 11, 14, -9}, 8);
    gmsh::model::geo::addSurfaceFilling({8}, 8);    

    //создание контуров поверхностей и объемов для тора
    gmsh::model::geo::addSurfaceLoop({1, 2, 3, 4}, 1); 
    gmsh::model::geo::addSurfaceLoop({5, 6, 7, 8}, 2);
    gmsh::model::geo::addVolume({1, 2});

    //синхронизация геометриии
    gmsh::model::geo::synchronize();

    //генерация сетки
    gmsh::model::mesh::generate(3);

    gmsh::write("torus.msh");

    std::set<std::string> args(argv, argv + argc);
    if(!args.count("-nopopup")) gmsh::fltk::run();

    gmsh::finalize(); 
}  
