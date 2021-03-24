#include<bits/stdc++.h>

const double color[][3] = {{0.4, 0.4, 0.4}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 0.0},
						   {1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, {0.5, 0.0, 0.0}, {0.0, 0.5, 0.0}, {0.0, 0.0, 0.5}, {0.0, 0.5, 0.5}, {0.5, 0.0, 0.5},
						   {0.5, 0.5, 0.0}, {0.8, 0.8, 0.8}, {0.69, 0.75, 0.10}, {0.82, 1.0, 0.08}, {0.91, 0.84, 0.42}, {1.0, 0.67, 0.11}, {0.50, 1.0, 0.0}, {1.0, 0.70, 0.0}};

enum colors{DARK_GREY, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, BLACK, DARK_RED, DARK_GREEN, DARK_BLUE, DARK_CYAN, DARK_MAGENTA,
            DARK_YELLOW, LIGHT_GREY, ACID_GREEN, ARCTIC_LIME, ARYLIDE_YELLOW, BRIGHT_YELLOW, CHARTREUSE, CHINESE_YELLOW};

struct coordinate {
	double x, y, z;
	coordinate(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};