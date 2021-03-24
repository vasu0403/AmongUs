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
int c = 0;
typedef tree <ll, null_type, less <ll>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;
// order_of_key(val): returns the number of values less than val
// find_by_order(k): returns an iterator to the kth largest element (0-based)
double PI = 3.141592653589793238;
double angle = PI / 5;


void print_vertex(coordinate v, int color_ind) {
    fstream f_object;
    f_object.open("decagonalPrism.txt", ios::app);
	f_object << v.x << "\n" << v.y << "\n" << v.z << "\n" << color[color_ind][0] << "\n" << color[color_ind][1] << "\n" << color[color_ind][2] << "\n";
    f_object.close();

}

void print_triangle(coordinate v1, coordinate v2, coordinate v3, int color_ind) {
	c++;
	print_vertex(v1, color_ind);
	print_vertex(v2, color_ind);
	print_vertex(v3, color_ind);
}

int main(void)
{
	srand(time(NULL));
	ios_base::sync_with_stdio(false);
    cin.tie(NULL);
	fstream f_object;
	f_object.open("decagonalPrism.txt", ios::out);
	f_object.close();
    double length = (double)0.5;
    double x1 = 0, y1 = 0;
    double vx = length, vy = 0;
    vector<coordinate> vertices;
    double up = (double) 0.25;
    vertices.pb({x1, y1, up});
    for(int i = 0; i < 9;i++) {
    	double x2 = x1 + vx, y2 = y1 + vy;
    	x1 = x2, y1 = y2;
    	double tx = vx*cos(angle) - vy*sin(angle);
    	double ty = vx*sin(angle) + vy*cos(angle);
    	vx = tx;
    	vy = ty; 
    	vertices.push_back(coordinate(x1, y1, up));
    }
    for(int i = 0; i < 10;i++) {
    	vertices.push_back(coordinate(vertices[i].x, vertices[i].y, -up));
    }
    for(int i = 2; i < 10; i++) {
		print_triangle(vertices[0], vertices[i], vertices[i - 1], RED);
    }
    for(int i = 12; i < 20; i++) {
		print_triangle(vertices[10], vertices[i], vertices[i - 1], RED);
    }
	int which[] = {CYAN, GREEN, CYAN, YELLOW, CYAN, DARK_GREEN, CYAN, DARK_YELLOW, CYAN, ACID_GREEN};

    for(int i = 0; i < 10; i++) {
    	int v1 = i, v2 = i + 1;
    	int v3 = v1 + 10, v4 = v2 + 10;
    	if(i == 9) {
    		v2 = 0, v4 = 10;
    	}
		print_triangle(vertices[v1], vertices[v3], vertices[v4], which[i]);
		print_triangle(vertices[v1], vertices[v2], vertices[v4], which[i]);

    }
}