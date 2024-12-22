#include <bits/stdc++.h>

using namespace std;

const double PI = 3.1415926535897932384626433;
const double EPS = 1e-9;

struct virus{
    double angle;
    bool start;  // true - початок, false - кінець

    inline bool operator < (const virus& e) const{
        if(fabs(angle - e.angle) < EPS){
            return start > e.start;  // спочатку йдуть початки, потім кінці
        }
        return angle < e.angle;
    }
};

double r, l;
int k;
vector<tuple<double, double, double, double> > rockets;


double get_distance(double x1, double y1, double x2, double y2){
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


tuple<double, double, double> find_line_equation(double x1, double y1, double x2, double y2) {
    double A = y2-y1;
    double B = -(x2-x1);
    double C = (x2-x1) * y1 - (y2-y1) * x1;
    return {A, B, C};
}


pair<double, double> chooseClosestPoint(double x0, double y0, double ax, double ay, double bx, double by) {
    double distA = get_distance(x0, y0, ax, ay);
    double distB = get_distance(x0, y0, bx, by);
    return (distA < distB) ? make_pair(ax, ay) : make_pair(bx, by);
}


pair<double, double> findIntersection(double x, double y) {
    // знаходимо коефіціенти прямої Ax + Bx + C заданих (x,y) та центром (0,0)
    auto [a, b, c] = find_line_equation(x, y, 0, 0);

    // стаття на cp.algo як знайти перетин кола та прямої, красивим чином:
    //https://cp-algorithms.com/geometry/circle-line-intersection.html

    // найближча точка до центра кола, яка належить прямій
    double x0 = -a*c/(a*a+b*b), y0 = -b*c/(a*a+b*b);

    // перетин кола і прямої в двох точках
    double d = r * r - c * c / (a * a + b * b);
    double mult = sqrt(d / (a * a + b * b));
    double ax = x0 + b * mult;
    double ay = y0 - a * mult;
    double bx = x0 - b * mult;
    double by = y0 + a * mult;

    // з цих двох точок: найближча до х,y - це і є та що влучить у планету
    return chooseClosestPoint(x, y, ax, ay, bx, by);
}


double calculate_angle(double x, double y) {
    return atan2(y, x); // функція для отримання кута, що відповідає точці (x, y) (відносно осі Ox)
}

void angleCorrection(double &startAngle, double &endAngle, int &active){
    bool f = 0;
    if(startAngle < 0){
        startAngle += 2*PI;
        f = 1;
    }
    if(fabs(startAngle - 2*PI) < EPS) startAngle = 0;

    if(endAngle < 0) endAngle += 2*PI;
    if(endAngle > 2*PI) endAngle -= 2*PI;

    // якщо початок дуги лежить перед вісю Ox, а кінець після
    if(f && endAngle < startAngle) ++active;
}


bool check(double time){
    vector<virus> segments;

    // відстежуємо кількість активних вірусів
    int active = 0;

    for(auto [x, y, rocketSpeed, virusSpeed]: rockets){
        double distance = sqrt(x * x + y * y);

        // час до зіткнення ракети з планетою
        double timeToImpact = (distance - r) / rocketSpeed;
        double currTime = time - timeToImpact;
        if(currTime < 0) continue; // ракета за цей час не досягнула планети

        auto [px, py] = findIntersection(x, y);
        double angle = calculate_angle(px, py);

        // вірус заразить усю планету за цей час
        double length = 2*virusSpeed * currTime;
        if(length >= 2*PI * r){
            ++active;
            continue;
        }

        double infectionAngle = virusSpeed * currTime / r; // дуга на колі, яку проходить вірус за цей час

        // кути, між якими поширюється вірус
        double startAngle = angle - infectionAngle;
        double endAngle = angle + infectionAngle;

        // коригуємо кути, щоб вони знаходились в межах [0, 2π)
        angleCorrection(startAngle, endAngle, active);

        // додаємо початок та кінець поширення вірусу
        segments.push_back({startAngle, true});  // початок

        // якщо кінець співпадає з кутом 0, то ми його не враховуємо
        if(fabs(endAngle - 2*PI) < EPS) {
            continue;
        }

        segments.push_back({endAngle, false});   // кінець
    }

    // сортуємо події за кутами
    sort(segments.begin(), segments.end());

    // якщо найперший відрізок не починається з кута 0, значить маємо перевірити цю діляку між кутом 0 і першим початком відрізка
    if(active <= l && !segments.empty() && segments[0].angle > 0){
        return 0;
    }
    // не можемо збити віруси, які заразили всю планету
    // fffffff segments and not rockets come on bitch, slandered all my time
    if(segments.empty() && active <= l) return 0;
    for(auto& event : segments) {
        if(event.start){
            ++active;
        }
        else{
            --active;
        }
        if(active <= l){
            return 0;
        }
    }
    return 1;
}

signed main() {
    ios::sync_with_stdio(false);cin.tie(0);
    // Input
    cin >> r >> k >> l;
    rockets.resize(k);
    for(auto &[cordx, cordy, rocketSpeed, virusSpeed] : rockets){
        cin >> cordx >> cordy >> rocketSpeed >> virusSpeed;
    }

    // binary search the time
    double low = 0, high = 1e7;
    while(high - low > EPS)
    {
        double mid = (low + high) / 2;
        if(check(mid)) high = mid;
        else low = mid;
    }

    cout.precision(10);
    cout << fixed << low << "\n";

    return 0;
}

