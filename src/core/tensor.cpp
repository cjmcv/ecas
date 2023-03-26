/*!
* \brief Tensor. 
*/

#include "tensor.hpp"

#include <iostream>
#include <string.h>
#include "util/logger.hpp"
#include "util/common.hpp"

namespace ecas {

Tensor::Tensor(std::vector<int> &shape, DataType type, void *data) {
    id_ = -1;
    shape_ = shape;
    type_ = type;

    TYPE_SWITCH(type, T, size_ = sizeof(T););
    for (int i=0; i < shape.size(); i++) {
        size_ *= shape[i];
    }
    if (size_ == 0)
        std::abort();
    
    device_buffer_ = nullptr;    
    host_buffer_ = new Buffer(size_, data);
    mode_ = ON_HOST;
}

Tensor::~Tensor() {
    if (host_buffer_ != nullptr) {
        delete host_buffer_;
        host_buffer_ = nullptr;      
    }
    if (device_buffer_ != nullptr) {
        delete device_buffer_;
        device_buffer_ = nullptr;      
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
    host_buffer_->SetDataPtr(data);
}

void *Tensor::GetData(MemoryMode mode) {
    if (mode == ON_HOST) {
        if (mode_ == ON_HOST)
            return host_buffer_->data();
        else {
            // TODO: push data from host to device.
            return device_buffer_->data();
        }
    }
    else {
        if (mode_ == ON_HOST) {
            // TODO: push data from device to host
            return host_buffer_->data();
        }
        else {
            return device_buffer_->data();
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