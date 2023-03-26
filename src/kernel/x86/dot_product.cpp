
#include "ecas/ecas.hpp"

namespace ecas {

void Dot(int len, const float *vec_a, const float *vec_b, float *res) {
	float temp = 0;
	for (int i = 0; i < len; i++) {
		temp += vec_a[i] * vec_b[i];
	}
	*res = temp;
}

} // ecas.