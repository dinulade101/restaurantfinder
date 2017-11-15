#include "restaurant.h"

/*
	Sets *ptr to the i'th restaurant. If this restaurant is already in the cache,
	it just copies it directly from the cache to *ptr. Otherwise, it fetches
	the block containing the i'th restaurant and stores it in the cache before
	setting *ptr to it.
*/
void getRestaurant(restaurant* ptr, int i, Sd2Card* card, RestCache* cache) {
	uint32_t block = REST_START_BLOCK + i/8;
	if (block != cache->cachedBlock) {
		while (!card->readBlock(block, (uint8_t*) cache->block)) {
			Serial.print("readblock failed, try again");
		}
		cache->cachedBlock = block;
	}
	*ptr = cache->block[i%8];
}

// Swaps the two restaurants (which is why they are pass by reference).
void swap(RestDist& r1, RestDist& r2) {
	RestDist tmp = r1;
	r1 = r2;
	r2 = tmp;
}

// Selection sort to sort the restaurants.
// void ssort(RestDist restaurants[]) {
// 	for (int i = NUM_RESTAURANTS-1; i >= 1; --i) {
// 		int maxId = 0;
// 		for (int j = 1; j <= i; ++j)
// 			if (restaurants[j].dist > restaurants[maxId].dist) {
// 				maxId = j;
// 			}
// 		swap(restaurants[i], restaurants[maxId]);
// 	}
// }

int pivot(RestDist restaurants[], int n, int pi) {
  swap(restaurants[pi], restaurants[n-1]);
  int lo = 0;
  int hi = n-2;

  // iterate until lo>high
  while (lo <= hi){
    if (restaurants[hi].dist > restaurants[n-1].dist) {
      hi--;
    }
    else if (restaurants[lo].dist <= restaurants[n-1].dist) {
      lo++;
    }
    else{
      swap(restaurants[lo], restaurants[hi]);
    }
  }
  
  swap(restaurants[lo], restaurants[n-1]);
  return lo;
}

//Sort an array with n elements using Quick Sort
void qsort(RestDist restaurants[], int n) {
  if (n < 1){return;}
  int pi = n/2;

  int newPi = pivot(restaurants, n, pi);

  qsort(restaurants, newPi);
  qsort(restaurants + (newPi + 1), n - (newPi + 1));

}

// Computes the manhattan distance between two points (x1, y1) and (x2, y2).
int16_t manhattan(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	return abs(x1-x2) + abs(y1-y2);
}

/*
	Fetches all restaurants from the card, saves their RestDist information
	in restaurants[], and then sorts them based on their distance to the
	point on the map represented by the MapView.
*/
void getAndSortRestaurants(const MapView& mv, RestDist restaurants[], Sd2Card* card, RestCache* cache) {
	restaurant r;

	// first get all the restaurants and store their corresponding RestDist information.
	for (int i = 0; i < NUM_RESTAURANTS; ++i) {
		getRestaurant(&r, i, card, cache);
		restaurants[i].index = i;
		restaurants[i].dist = manhattan(lat_to_y(r.lat), lon_to_x(r.lon),
																		mv.mapY + mv.cursorY, mv.mapX + mv.cursorX);
	}

	// Now sort them.
	qsort(restaurants, NUM_RESTAURANTS);
}
