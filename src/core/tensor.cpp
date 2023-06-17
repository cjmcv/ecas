/*!
* \brief Tensor. 
*/

#include "tensor.hpp"

#include <iostream>
#include <string.h>
#include "util/logger.hpp"
#include "util/common.hpp"
#include "backend/buffer/host_buffer.hpp"

namespace ecas {

Tensor::Tensor(std::vector<int> &shape, DataType type) {
    id_ = -1;
    shape_ = shape;
    type_ = type;

    TYPE_SWITCH(type, T, size_ = sizeof(T););
    for (int i=0; i < shape.size(); i++) {
        size_ *= shape[i];
    }
    if (size_ == 0)
        std::abort();
    
    is_owned_buffer_ = false;
    buffer_ = nullptr;
    mode_ = ON_HOST;
}

Tensor::~Tensor() {
    if (is_owned_buffer_ == true && 
        buffer_ != nullptr) {

        delete buffer_;
        buffer_ = nullptr;      
    }
}

void Tensor::CheckDimension(ITensor *target) {
    if (shape_.size() != target->shape().size()) {
        ECAS_LOGE("Tensor::CloneFrom -> shape mismatch.\n");
    }
    for (int i=0; i<shape_.size(); i++) {
        if (shape_[i] != target->shape()[i]) {
            ECAS_LOGE("Tensor::CloneFrom -> shape mismatch.\n");
        }
    }
}

void Tensor::BindBuffer(Buffer *buffer) {
    buffer_ = buffer;
}

void Tensor::CopyFrom(ITensor *in) {
    // Check dimension.
    CheckDimension(in);
    // Check memory type,
    if (mode_ != in->mode()) {
        ECAS_LOGE("Tensor::CloneFrom -> memory type mismatch.\n");
    }
    id_ = in->id();
    memcpy(GetData(), in->GetData(), size_);
}

void Tensor::CopyTo(ITensor *out) {
    // Check dimension.
    CheckDimension(out);
    // Check memory type,
    if (mode_ != out->mode()) {
        ECAS_LOGE("Tensor::CopyTo -> memory type mismatch.\n");
    }
    out->SetId(id_);
    memcpy(out->GetData(), GetData(), size_);
}

void Tensor::BindHostDataPtr(void *data) {
    if (buffer_ == nullptr) {
        ECAS_LOGE("Tensor::BindHostDataPtr -> buffer_ == nullptr.\n");
    }

    // 只能持有不含内存的host buffer，其他包含内存的buffer均不持有
    is_owned_buffer_ = true; 
    buffer_ = new HostBuffer(size_, data);
}

void *Tensor::GetData(MemoryMode mode) {
    if (mode == ON_HOST) {
        if (mode_ == ON_HOST)
            return buffer_->data();
        else {
            // TODO: push data from host to device.

        }
    }
    else {
        if (mode_ == ON_HOST) {
            // TODO: push data from device to host
            return buffer_->data();
        }
        else {

        }
    }
}

void Tensor::Print() {
    ECAS_LOGS("\n====== Tensor %p ======\n", this);
    ECAS_LOGS("\nShape: ");
    for (int i = 0; i < shape_.size(); i++) {
        ECAS_LOGS("%d, ", shape_[i]);
    }

    ECAS_LOGS("\nData: \n");
    int s[4] = {1, 1, 1, 1};
    memcpy(s + 4 - shape_.size(), &shape_[0], sizeof(uint32_t) * shape_.size());

    void *host_data = GetData(ON_HOST);
    TYPE_SWITCH(type_, T, {
        T *data = (T *)host_data;
        for (int n = 0; n < s[0]; n++) {
            int n_bias = n * s[1] * s[2] * s[3];
            for (int c = 0; c < s[1]; c++) {
                int c_bias = c * s[2] * s[3];
                ECAS_LOGS("(n: %d, c: %d)\n", n, c);
                for (int h = 0; h < s[2]; h++) {
                    int h_bias = h * s[3];
                    for (int w = 0; w < s[3]; w++) {
                        std::cout << data[n_bias + c_bias + h_bias + w] << ", ";
                    }
                }
            }
        }
    });
}

}  // end of namespace ecas.