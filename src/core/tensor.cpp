/*!
* \brief Tensor. 
*/

#include "tensor.hpp"

#include <string.h>
#include "util/logger.hpp"
#include "util/common.hpp"

namespace ecas {

Tensor::Tensor(std::vector<int> &shape, DataType type) {
    it_ = new ITensor();
    it_->id = -1;
    it_->shape = shape;
    it_->type = type;
    it_->mode = ON_HOST;

    TYPE_SWITCH(type, T, size_ = sizeof(T););
    for (int i=0; i < shape.size(); i++) {
        size_ *= shape[i];
    }
    if (size_ == 0)
        std::abort();
    
    buffer_ = new Buffer(size_);
    it_->data = (char *)buffer_->data();

    it_->mode = ON_HOST;
}

// Tensor::Tensor(ITensor &in) {
//     size_ = 1;
//     for (int i=0; i < in.shape.size(); i++) {
//         size_ *= in.shape[i];
//     }
//     if (size_ == 0)
//         std::abort();

//     shape_ = in.shape;
//     buffer_ = new Buffer(in.data, size_);
// }

Tensor::~Tensor() {
    delete buffer_;
}

void Tensor::CheckDimension(ITensor *target) {
    if (it_->shape.size() != target->shape.size()) {
        ECAS_LOGE("Tensor::CloneFrom -> shape mismatch.\n");
    }
    for (int i=0; i<it_->shape.size(); i++) {
        if (it_->shape[i] != target->shape[i]) {
            ECAS_LOGE("Tensor::CloneFrom -> shape mismatch.\n");
        }
    }
}

void Tensor::CopyFrom(ITensor *in) {
    // Check dimension.
    CheckDimension(in);
    // Check memory type,
    if (it_->mode != in->mode) {
        ECAS_LOGE("Tensor::CloneFrom -> memory type mismatch.\n");
    }
    it_->id = in->id;
    memcpy(it_->data, in->data, size_);
}

void Tensor::CopyTo(ITensor *out) {
    // Check dimension.
    CheckDimension(out);
    // Check memory type,
    if (it_->mode != out->mode) {
        ECAS_LOGE("Tensor::CopyTo -> memory type mismatch.\n");
    }
    out->id = it_->id;
    memcpy(out->data, it_->data, size_);
}

}  // end of namespace ecas.