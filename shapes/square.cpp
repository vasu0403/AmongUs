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


double dx[] = {-1, 1, 1, -1};
double dy[] = {-1, -1, 1, 1};


void print_vertex(coordinate v) {
    fstream f_object;
    f_object.open("square.txt", ios::app);
	f_object << v.x << "\n" << v.y << "\n" << v.z << "\n";
    f_object.close();

}

void print_triangle(coordinate v1, coordinate v2, coordinate v3) {
	print_vertex(v1);
	print_vertex(v2);
	print_vertex(v3);
}

int main(void)
{
	ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    fstream f_object;
    f_object.open("square.txt", ios::out);
    f_object.close();
    vector<coordinate> lower, upper;

    lower.push_back(coordinate(-0.5, -0.5, -0.5)), lower.push_back(coordinate(+0.5, -0.5, -0.5));
    lower.push_back(coordinate(+0.5, +0.5, -0.5)), lower.push_back(coordinate(-0.5, +0.5, -0.5));

    upper.push_back(coordinate(-0.5, -0.5, +0.5)), upper.push_back(coordinate(+0.5, -0.5, +0.5));
    upper.push_back(coordinate(+0.5, +0.5, +0.5)), upper.push_back(coordinate(-0.5, +0.5, +0.5));

   	for(int i = 0; i < 4; i++) {
   		int ind1 = i, ind2 = (i + 1) % 4;

   		print_triangle(lower[ind1], lower[ind2], upper[ind1]);
   		print_triangle(upper[ind1], upper[ind2], lower[ind2]);

   	}
	int ind = 0;
	print_triangle(lower[ind], lower[ind + 1], lower[ind + 2]);
	print_triangle(lower[ind + 2], lower[ind + 3], lower[ind]);

	print_triangle(upper[ind], upper[ind + 1], upper[ind + 2]);
	print_triangle(upper[ind + 2], upper[ind + 3], upper[ind]);


}