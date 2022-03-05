

#ifndef CODE_CONVEXHULLALGORITHMS_H
#define CODE_CONVEXHULLALGORITHMS_H

#include "CGALComponents.h"
#include <QtWidgets>
#include <QApplication>
#include <QLabel>
#include <QString>
#include <QTranslator>
#include <CGAL/convex_hull_2.h>
#include <stack>
void jarvisMarch(const std::vector<Point> &P, std::vector<unsigned> &idsOfConvexHullPoints);
void grahamsScan(const std::vector<Point> &P, std::vector<unsigned> &idsOfConvexHullPoints);

void drawConvexHullUsingQT(const std::vector<Point> &P,
                           const std::vector<Point> &convexHullPoints,
                           const bool labels) {

    const double pointSize = 4; // adjust this value to change the size of the points
    /***************************************************/
    QPicture pi;
    QPainter canvas(&pi);
    canvas.setRenderHint(QPainter::Antialiasing);
    //canvas.setFont(QFont("Times", 12));
    // DO NOT TOUCH THESE Qt STATEMENTS!!!
    /***************************************************/

    std::vector<QPointF> verticesForQTConvexHull;
    verticesForQTConvexHull.reserve(convexHullPoints.size());
    for( Point p : convexHullPoints )
        verticesForQTConvexHull.emplace_back( QPointF(p.x(),p.y() ) );

    canvas.drawPolygon(&verticesForQTConvexHull[0], (int)verticesForQTConvexHull.size());

    unsigned id = 0;
    for ( Point p : P ) {
        canvas.setBrush(Qt::black);

        canvas.drawEllipse(QPointF(p.x(), p.y()), pointSize, pointSize);
        if(labels) {
            QPointF textPos(p.x() + 4.0, p.y() + 4.0);
            canvas.drawText(textPos, QString(std::to_string(id).c_str()));
        }
        id++;
    }

    for ( Point p : convexHullPoints ) {
        canvas.setBrush(Qt::blue);
        canvas.drawEllipse(QPointF(p.x(), p.y()), pointSize, pointSize);
    }

    /*********************************************/
    canvas.end();
    auto l = new QLabel;
    l->setStyleSheet("QLabel { background-color : white; color : black; }");
    l->setPicture(pi);
    l->setWindowTitle("Polygon ConvexHull");
    l->show();
    // l->showMaximized();
    QApplication::exec();
    // DO NOT TOUCH THESE Qt STATEMENTS!!!
    /***************************************************/
}

void plot() {

    std::ofstream file ("a.txt");
    file << "n-value" << "\t" << "grahamsScan" << "\t" << "jarvisMarch" << std::endl;

    for(int i = 0; i < 20; i++){
        double grahamTime;
        double jarvisTime;
        for(int j = 0; j < 5; j++) {
            unsigned n = (i + 1) * 1000;
            double sizeOfSquare = 1000;
            std::vector<Point> P;
            std::vector<unsigned> idsOfConvexHullPoints;
            generatePointsInsideASquare(n, sizeOfSquare, P);

            CGAL::Timer timer;
            timer.start();
            grahamsScan(P, idsOfConvexHullPoints);
            timer.stop();
            grahamTime += timer.time() * 1000;

            timer.reset();
            timer.start();
            jarvisMarch(P, idsOfConvexHullPoints);
            timer.stop();
            jarvisTime += timer.time() * 1000;
        }
        file << (i+1)*1000 << "\t" << grahamTime/5 << "ms" << "\t" << jarvisTime/5 << "ms" << std::endl;
    }
    file.close();
    file.open("b.txt");
    file << "n-value" << "\t" << "grahamsScan" << "\t" << "jarvisMarch" << std::endl;
    for(int i = 0; i < 20; i++){
        double grahamTime;
        double jarvisTime;
        for(int j = 0; j < 5; j++) {
            unsigned n = (i + 1) * 1000;
            double sizeOfSquare = 1000;
            std::vector<Point> P;
            std::vector<unsigned> idsOfConvexHullPoints;
            generateConvexPointSetInsideASquare(n, sizeOfSquare, P);

            CGAL::Timer timer;
            timer.start();
            grahamsScan(P, idsOfConvexHullPoints);
            timer.stop();
            grahamTime += timer.time() * 1000;

            timer.reset();
            timer.start();
            jarvisMarch(P, idsOfConvexHullPoints);
            timer.stop();
            jarvisTime += timer.time() * 1000;
        }
        file << (i+1)*1000 << "\t" << grahamTime/5 << "ms" << "\t" << jarvisTime/5 << "ms" << std::endl;
    }
    file.close();
}

void jarvisMarch(const std::vector<Point> &P, std::vector<unsigned> &idsOfConvexHullPoints) {

    if(P.size() < 3){
        return;
    }

    std::vector<Point> hull;
    unsigned left = 0;
    for(unsigned i = 1; i < P.size(); i++){
        if(P[i].x() < P[left].x()){
            left = i;
        }
    }
    unsigned p = left, temp;
    do{
        hull.push_back((P[p]));
        temp = (p+1)%2;
        for(unsigned i = 0; i < P.size(); i++){
            if(orientation(P[p], P[i], P[temp]) == CGAL::LEFT_TURN){
                temp = i;
            }
        }
        p = temp;
    }while(p != left);

    auto pointsToIdMap = createMapOfPoints(P);
    for(auto & i : hull) {
        idsOfConvexHullPoints.push_back(pointsToIdMap[i]);
    }
}
bool compare(const Point &a, const Point &b)
{
    return (a.x() < b.x()) || (a.x() == b.x() && a.y() < b.y());
}
void grahamsScan(const std::vector<Point> &P, std::vector<unsigned> &idsOfConvexHullPoints) {
    auto pointsToIdMap = createMapOfPoints(P);
    std::vector<Point> points = P;
    unsigned n = points.size();
    sort(points.begin(), points.end(), compare); //Sort by x-coordinate
    std::reverse(points.begin(), points.end());
    std::vector<Point> Lupper; //create empty list
    Lupper.push_back(points[0]); //first point is p1
    Lupper.push_back(points[1]); //second point is p2

    for(unsigned i = 2; i < n; i++){
        while(Lupper.size() >= 2 && CGAL::orientation(Lupper[Lupper.size()-2], Lupper[Lupper.size()-1], points[i]) != CGAL::RIGHT_TURN){
            Lupper.pop_back();
        }
        Lupper.push_back(points[i]);
    }

    std::vector<Point> Llower; //create empty list
    Llower.push_back(points[n-1]); //first point is pn
    Llower.push_back(points[n-2]); //second point is pn-1

    for(unsigned i = n-2; i >= 1; i--){
        while(Llower.size() >= 2 && CGAL::orientation(Llower[Llower.size()-2], Llower[Llower.size()-1], points[i-1]) != CGAL::RIGHT_TURN){
            Llower.pop_back();
        }
        Llower.push_back(points[i-1]);
    }
    Llower.pop_back();
    Llower.erase(Llower.begin());



    std::vector<Point> hull = Lupper;
    hull.insert(hull.end(), Llower.begin(), Llower.end());

    for(auto & i : hull) {
        idsOfConvexHullPoints.push_back(pointsToIdMap[i]);
    }



}
void correctnessCheck(){
    unsigned jarvisFails = 0, grahamFails = 0;

    std::cout << "Checking for random points within a square..." << std::endl;

    for(unsigned n = 1; n <= 20; ++n) {
        std::cout << "\nn: " << n << "K, " ;
        for (unsigned i = 0; i < 5; ++i) {
            std::cout << " Run: " << i + 1 << " ";

            std::vector<Point> P;
            generatePointsInsideASquare(n * 1000, 500, P);

            std::vector<Point> copyOfP;
            std::unordered_set<Point> hull;
            std::copy(P.begin(), P.end(), back_inserter(copyOfP));
            CGAL::convex_hull_2(copyOfP.begin(), copyOfP.end(), std::inserter(hull, hull.end()));

            copyOfP.clear();
            std::copy(P.begin(), P.end(), back_inserter(copyOfP));
            std::vector<unsigned> idsOfConvexHullPointsForJarvis;
            jarvisMarch(copyOfP, idsOfConvexHullPointsForJarvis);

            if( hull.size() != idsOfConvexHullPointsForJarvis.size() )
                jarvisFails++;
            else {
                for( unsigned id : idsOfConvexHullPointsForJarvis )
                    if( hull.count(P[id]) != 1) {
                        jarvisFails++;
                        break;
                    }
            }

            copyOfP.clear();
            std::copy(P.begin(), P.end(), back_inserter(copyOfP));
            std::vector<unsigned> idsOfConvexHullPointsForGraham;
            grahamsScan(copyOfP, idsOfConvexHullPointsForGraham);

            if( hull.size() != idsOfConvexHullPointsForGraham.size() )
                grahamFails++;
            else {
                for( unsigned id : idsOfConvexHullPointsForGraham )
                    if( hull.count(P[id]) != 1) {
                        grahamFails++;
                        break;
                    }
            }
        }
    }

    std::cout << "\n\nChecking for random convex point sets within a square..." << std::endl;

    for(unsigned n = 1; n <= 20; ++n) {
        std::cout << "\nn: " << n << "K, " ;
        for (unsigned i = 0; i < 5; ++i) {
            std::cout << " Run: " << i + 1 << " ";

            std::vector<Point> P;
            generateConvexPointSetInsideASquare(n * 1000, 500, P);

            std::vector<Point> copyOfP;
            std::unordered_set<Point> hull;
            std::copy(P.begin(), P.end(), back_inserter(copyOfP));
            CGAL::convex_hull_2(copyOfP.begin(), copyOfP.end(), std::inserter(hull, hull.end()));

            copyOfP.clear();
            std::copy(P.begin(), P.end(), back_inserter(copyOfP));
            std::vector<unsigned> idsOfConvexHullPointsForJarvis;
            jarvisMarch(copyOfP, idsOfConvexHullPointsForJarvis);

            if( hull.size() != idsOfConvexHullPointsForJarvis.size() )
                jarvisFails++;
            else {
                for( unsigned id : idsOfConvexHullPointsForJarvis )
                    if( hull.count(P[id]) != 1) {
                        jarvisFails++;
                        break;
                    }
            }

            copyOfP.clear();
            std::copy(P.begin(), P.end(), back_inserter(copyOfP));
            std::vector<unsigned> idsOfConvexHullPointsForGraham;
            grahamsScan(copyOfP, idsOfConvexHullPointsForGraham);

            if( hull.size() != idsOfConvexHullPointsForGraham.size() )
                grahamFails++;
            else {
                for( unsigned id : idsOfConvexHullPointsForGraham )
                    if( hull.count(P[id]) != 1) {
                        grahamFails++;
                        break;
                    }
            }
        }
    }

    std::cout << "\n\nNumber of times Jarvis March has failed: " << jarvisFails << std::endl;
    std::cout << "Number of times Graham's Scan has failed: " << grahamFails << std::endl;
}
#endif //CODE_CONVEXHULLALGORITHMS_H