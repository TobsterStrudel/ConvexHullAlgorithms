#include "CGALComponents.h"
#include "ConvexHullAlgorithms.h"

int main(int argc, char *argv[]) {
    correctnessCheck();
//    plot();
    QApplication a(argc, argv);
    unsigned n = 20;
    double sizeOfSquare = 500;
    std::vector<Point> P;
    std::vector<unsigned> idsOfConvexHullPoints;
    generateConvexPointSetInsideASquare(n, sizeOfSquare, P);

//    generatePointsInsideASquare(n, sizeOfSquare, P);
    grahamsScan(P, idsOfConvexHullPoints);
//    jarvisMarch(P, idsOfConvexHullPoints);
    std::vector<Point> convexHull;

    for(unsigned i : idsOfConvexHullPoints){
        convexHull.push_back(P[i]);
    }
    drawConvexHullUsingQT(P, convexHull, true);

    return 0;
}
