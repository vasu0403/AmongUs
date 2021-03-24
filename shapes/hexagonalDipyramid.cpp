#include "bits/stdc++.h"
#include "ext/pb_ds/assoc_container.hpp"
#include "ext/pb_ds/tree_policy.hpp"
#include "colors.h"
using namespace std;
using namespace __gnu_pbds;


#define ll long long int
#define pb push_back
#define mp make_pair
#define ff first
#define ss second
#define all(a) a.begin(),a.end()

typedef tree <ll, null_type, less <ll>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;
// order_of_key(val): returns the number of values less than val
// find_by_order(k): returns an iterator to the kth largest element (0-based)

double PI = 3.141592653589793238;
double angle = PI / 3.0;

void print_vertex(coordinate v, int color_ind) {
    fstream f_object;
    f_object.open("hexagonalDipyramid.txt", ios::app);
	f_object << v.x << "\n" << v.y << "\n" << v.z << "\n" << color[color_ind][0] << "\n" << color[color_ind][1] << "\n" << color[color_ind][2] << "\n";
    f_object.close();

}

void print_triangle(coordinate v1, coordinate v2, coordinate v3, int color_ind) {
	print_vertex(v1, color_ind);
	print_vertex(v2, color_ind);
	print_vertex(v3, color_ind);
}

int main(void)
{
    fstream f_object;
    f_object.open("hexagonalDipyramid.txt", ios::out);
    f_object.close();
    double height = 0.4;
    double length = 0.3;
    coordinate upper = coordinate(0, 0, height);
    coordinate lower = coordinate(0, 0, -height);

    coordinate first = coordinate(0, length, 0);
    vector<coordinate> hexagon = {first};

    for(int i = 1; i < 6; i++) {
        coordinate last = hexagon.back();
        coordinate next = coordinate(cos(angle) * last.x - sin(angle) * last.y, sin(angle) * last.x + cos(angle) * last.y, 0);
        hexagon.push_back(next);
    }
    int colors[] = {MAGENTA, YELLOW, WHITE, BLACK, MAGENTA, YELLOW, WHITE, BLACK, MAGENTA, YELLOW, WHITE, BLACK};
    for(int i = 0; i < 6; i++) {
        int ind1 = i;
        int ind2 = (i + 1) % 6;
        print_triangle(hexagon[ind1], hexagon[ind2], upper, colors[2 * i]);
        print_triangle(hexagon[ind1], hexagon[ind2], lower, colors[2 * i + 1]);
    }
}