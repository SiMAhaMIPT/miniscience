#include <iostream>
#include <set>

// Gmsh C++ API полностью определена в заголовочном файле "gmsh.h" (который содержит
// полную документацию всех функций в API):
#include <gmsh.h>

int main(int argc, char **argv)
{
  // Для использования функций в C++ API Gmsh должен быть инициализирован:
  gmsh::initialize();

  // Добавляем новую модель "t1". Если gmsh::model::add() не вызывается, будет создана
  // новая модель по умолчанию (без имени), при необходимости.
  gmsh::model::add("t1");

  // API C++ предоставляет прямой доступ к каждому поддерживаемому ядру геометрии (CAD).
  // В этом первом учебнике используется встроенное ядро: соответствующие функции API
  // находятся в пространстве имен "gmsh::model::geo".

  // Первый тип "элементарной сущности" в Gmsh - "точка". Для создания
  // точки с встроенным ядром CAD, функция API C++ - gmsh::model::geo::addPoint():
  // - первые 3 аргумента - координаты точки (x, y, z)
  // - следующий (необязательный) аргумент - целевой размер сетки рядом с точкой
  // - последний (необязательный) аргумент - тег точки (строго положительное целое
  //   число, уникально идентифицирующее точку)
  double lc = 1e-2;
  gmsh::model::geo::addPoint(0, 0, 0, lc, 1);

  // Распределение размеров элементов сетки будет получено путем интерполяции этих
  // размеров сетки по всей геометрии. Другой метод задания размеров сетки - использовать
  // общие поля размеров сетки (см. `t10.cpp'). Особый случай - использование фоновой сетки
  // (см. `t7.cpp').
  //
  // Если целевой размер сетки не указан, будет использовано по умолчанию равномерное
  // крупное размер для модели, основанное на общем размере модели.
  //
  // Мы можем затем определить дополнительные точки. Все точки должны иметь разные теги:
  gmsh::model::geo::addPoint(.1, 0, 0, lc, 2);
  gmsh::model::geo::addPoint(.1, .3, 0, lc, 3);

  // Если тег не указан явно, создается новый тег автоматически и возвращается функцией:
  int p4 = gmsh::model::geo::addPoint(0, .3, 0, lc);

  // Линии - второй тип элементарных сущностей в Gmsh, и, среди линий, прямые линии
  // являются самыми простыми. API для создания прямых отрезков линий с встроенным ядром
  // следует тем же соглашениям: первые 2 аргумента - теги точек (начальная и конечная
  // точки линии), и последний (необязательный) - тег линии.
  //
  // В приведенных ниже командах, например, линия 1 начинается в точке 1 и заканчивается
  // в точке 2.
  //
  // Обратите внимание, что теги кривых отдельны от тегов точек - поэтому мы можем
  // повторно использовать тег `1' для нашей первой кривой. И как общее правило, теги
  // элементарных сущностей в Gmsh должны быть уникальными по геометрическим размерам.
  gmsh::model::geo::addLine(1, 2, 1);
  gmsh::model::geo::addLine(3, 2, 2);
  gmsh::model::geo::addLine(3, p4, 3);
  gmsh::model::geo::addLine(4, 1, p4);

  // Третья элементарная сущность - поверхность. Для определения простой
  // прямоугольной поверхности из четырех определенных выше кривых сначала
  // должен быть определен цикл кривой. Цикл кривой определяется упорядоченным
  // списком соединенных кривых, к каждой кривой присоединяется знак (в зависимости
  // от ориентации кривой для формирования цикла). Функция API для создания цикла
  // кривой принимает вектор целых чисел в качестве первого аргумента и тег цикла
  // кривой (который должен быть уникальным среди циклов кривых) в качестве второго
  // (необязательного) аргумента:
  gmsh::model::geo::addCurveLoop({4, 1, -2, 3}, 1);

  // Затем мы можем определить поверхность как список циклов кривых (здесь только один,
  // представляющий внешний контур, поскольку нет отверстий - см. `t4.cpp' для примера
  // поверхности с отверстием):
  gmsh::model::geo::addPlaneSurface({1}, 1);

  // Прежде чем их можно будет сеточить (и, более общим образом, использовать функции
  // API вне функций встроенного ядра CAD), геометрические сущности CAD должны быть
  // синхронизированы с моделью Gmsh, которая создаст соответствующие структуры данных
  // Gmsh. Это достигается с помощью вызова API gmsh.model.geo.synchronize() для встроенного
  // ядра CAD. Синхронизации могут вызываться в любое время, но они включают значительное
  // количество обработки; поэтому, хотя вы могли бы синхронизировать внутренние данные CAD
  // после каждой команды CAD, обычно лучше минимизировать количество точек синхронизации.
  gmsh::model::geo::synchronize();

  // На этом уровне Gmsh знает все для отображения прямоугольной поверхности 1 и для ее
  // сеточения. Если мы хотим сгруппировать элементарные геометрические сущности в более
  // значимые группы, например, чтобы определить некоторые математические ("область",
  // "граница"), функциональные ("левое крыло", "фюзеляж") или материальные ("сталь", "углерод")
  // свойства.
  //
  // Такие группы называются "Физические группы" в Gmsh. По умолчанию, если физические
  // группы определены, Gmsh будет экспортировать в выходные файлы только сеточные элементы,
  // которые принадлежат хотя бы одной физической группе. (Чтобы заставить Gmsh сохранить
  // все элементы, независимо от того, принадлежат они к физическим группам или нет,
  // установите `Mesh.SaveAll' в 1.) Физические группы также идентифицируются тегами,
  // т.е. строго положительными целыми числами, которые должны быть уникальными по
  // размерам геометрии (0D, 1D, 2D или 3D). Физические группы также могут иметь имена.
  //
  // Здесь мы определяем физическую кривую, группирующую левые, нижние и правые кривые в
  // одну группу (с предписанным тегом 5); и физическую поверхность с именем "Моя поверхность"
  // (с автоматическим тегом), содержащую геометрическую поверхность 1:
  //gmsh::model::addPhysicalGroup(1, {1, 2, 4}, 5);
  //int ps = gmsh::model::addPhysicalGroup(2, {1});
  //gmsh::model::setPhysicalName(2, ps, "Моя поверхность");

  // Мы можем затем создать сетку 2D...
  gmsh::model::mesh::generate(2);

  // ... и сохранить ее на диск
  gmsh::write("t1.msh");

  // По умолчанию, если физические группы определены, Gmsh будет экспортировать в файл
  // сетки только те элементы, которые принадлежат хотя бы одной физической группе. Чтобы
  // заставить Gmsh сохранить все элементы, вы можете использовать
  //
  // gmsh::option::setNumber("Mesh.SaveAll", 1);

  // По умолчанию Gmsh сохраняет сетки в последней версии формата файла сетки Gmsh
  // (формат `MSH'). Вы можете сохранить сетки в других форматах сетки, указав имя
  // файла с другим расширением. Например
  //
  //   gmsh::write("t1.unv");
  //
  // сохранит сетку в формате UNV. Вы также можете сохранить сетку в более старых
  // версиях формата MSH: просто установите
  //
  //   gmsh::option::setNumber("Mesh.MshFileVersion", x);
  //
  // для любого номера версии `x'. Как альтернативу, вы также можете не указывать
  // формат явно, а просто выбрать имя файла с расширением `.msh2' или `.msh4'.

  // Для визуализации модели мы можем запустить графический пользовательский интерфейс
  // с `gmsh::fltk::run()'. Здесь мы запускаем его только если в командной строке
  // не указано "-nopopup":
  std::set<std::string> args(argv, argv + argc);
  if(!args.count("-nopopup")) gmsh::fltk::run();

  // Обратите внимание, что начиная с Gmsh 3.0, модели можно строить с использованием
  // других ядер геометрии, чем стандартное "встроенное" ядро. Чтобы использовать ядро
  // CAD OpenCASCADE вместо встроенного ядра, вы должны использовать функции в
  // пространстве имен "gmsh::model::occ".
  //
  // Разные ядра CAD имеют разные функции. С OpenCASCADE, вместо определения поверхности
  // путем последовательного определения 4 точек, 4 кривых и 1 цикла кривой, можно
  // определить прямоугольную поверхность непосредственно с помощью
  //
  // gmsh::model::occ::addRectangle(.2, 0, 0, .1, .3);
  //
  // После синхронизации с моделью Gmsh с
  //
  // gmsh::model::occ::synchronize();
  //
  // базовые кривые и точки могут быть доступны с помощью
  // gmsh::model::getBoundary().
  //
  // См., например, `t16.cpp', `t18.cpp', `t19.cpp' или `t20.cpp' для полных примеров
  // на основе OpenCASCADE, и `demos/api' для более подробной информации.

  // Это должно быть вызвано, когда вы закончили использовать Gmsh C++ API:
  gmsh::finalize();

  return 0;
}
