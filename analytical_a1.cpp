#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <string>

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

struct Circle {
    Point center;
    double radius;
    Circle(Point c, double r) : center(c), radius(r) {}
};

bool isPointInCircle(const Point& p, const Circle& c) {
    double dx = p.x - c.center.x;
    double dy = p.y - c.center.y;
    return dx * dx + dy * dy <= c.radius * c.radius;
}

bool isPointInIntersection(const Point& p, const std::vector<Circle>& circles) {
    for (const auto& circle : circles) {
        if (!isPointInCircle(p, circle)) {
            return false;
        }
    }
    return true;
}

double monteCarloArea(const std::vector<Circle>& circles, 
                      double x_min, double x_max, 
                      double y_min, double y_max, 
                      int num_points, int seed = 42) 
{
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> x_dist(x_min, x_max);
    std::uniform_real_distribution<> y_dist(y_min, y_max);

    int points_inside = 0;

    for (int i = 0; i < num_points; ++i) {
        Point p(x_dist(gen), y_dist(gen));
        if (isPointInIntersection(p, circles)) {
            points_inside++;
        }
    }

    double area_rect = (x_max - x_min) * (y_max - y_min);
    return (static_cast<double>(points_inside) / num_points) * area_rect;
}

double calculateExactArea() {
    return 0.25 * M_PI + 1.25 * std::asin(0.8) - 1.0;
}

void runExperiment(const std::string& filename) {
    std::vector<Circle> circles = {
        Circle(Point(1.0, 1.0), 1.0),
        Circle(Point(1.5, 2.0), std::sqrt(5.0) / 2.0),
        Circle(Point(2.0, 1.5), std::sqrt(5.0) / 2.0)
    };

    double wide_x_min = 0.0, wide_x_max = 3.0;
    double wide_y_min = 0.0, wide_y_max = 3.0;
    double wide_area_rect = (wide_x_max - wide_x_min) * (wide_y_max - wide_y_min);

    double narrow_x_min = 1.0, narrow_x_max = 2.0;
    double narrow_y_min = 1.0, narrow_y_max = 2.0;
    double narrow_area_rect = (narrow_x_max - narrow_x_min) * (narrow_y_max - narrow_y_min);

    double exact_area = calculateExactArea();

    std::ofstream output(filename);
    output << "N,Wide_Area,Wide_Relative_Error,Narrow_Area,Narrow_Relative_Error,"
           << "Wide_Absolute_Error,Narrow_Absolute_Error\n";

    std::cout << "Запуск эксперимента..." << std::endl;
    std::cout << "Точная площадь: " << exact_area << std::endl;
    std::cout << "Площадь широкой области: " << wide_area_rect << std::endl;
    std::cout << "Площадь узкой области: " << narrow_area_rect << std::endl;

    for (int N = 100; N <= 100000; N += 500) {
        double wide_area = monteCarloArea(circles, wide_x_min, wide_x_max, 
                                          wide_y_min, wide_y_max, N, N);
        double wide_rel_error = std::abs(wide_area - exact_area) / exact_area;
        double wide_abs_error = std::abs(wide_area - exact_area);

        double narrow_area = monteCarloArea(circles, narrow_x_min, narrow_x_max, 
                                            narrow_y_min, narrow_y_max, N, N);
        double narrow_rel_error = std::abs(narrow_area - exact_area) / exact_area;
        double narrow_abs_error = std::abs(narrow_area - exact_area);

        output << N << "," << wide_area << "," << wide_rel_error << ","
               << narrow_area << "," << narrow_rel_error << ","
               << wide_abs_error << "," << narrow_abs_error << "\n";

        if (N % 10000 == 0) {
            std::cout << "Обработано N = " << N << std::endl;
        }
    }

    output.close();
    std::cout << "Эксперимент завершен. Результаты сохранены в " << filename << std::endl;
}

int main() {
    runExperiment("monte_carlo_results.csv");
    return 0;
}
