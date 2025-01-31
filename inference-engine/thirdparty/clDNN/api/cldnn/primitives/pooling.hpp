// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "primitive.hpp"
#include <vector>

namespace cldnn {
/// @addtogroup cpp_api C++ API
/// @{
/// @addtogroup cpp_topology Network Topology
/// @{
/// @addtogroup cpp_primitives Primitives
/// @{

/// @brief Select method for the @ref pooling layer.
enum class pooling_mode : int32_t {
    /// @brief Maximum-pooling method.
    max,
    /// @brief Average-pooling method - values.
    average,
    /// @brief Average-pooling method without values which are outside of the input.
    average_no_padding,
    /// @brief Maximum-pooling method with additional buffer to store argmax indices.
    max_with_argmax,
    /// @brief Pooling with bilinear interpolation.
    bilinear,
    /// @brief Deformable pooling with bilinear interpolation.
    deformable_bilinear
};

/// @brief Performs "pooling" operation which is a form of non-linear down-sampling.
/// @details Pools the input image by taking the max, average, etc. within regions.
struct pooling : public primitive_base<pooling> {
    CLDNN_DECLARE_PRIMITIVE(pooling)

    /// @brief Constructs pooling primitive.
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param mode Pooling mode.
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param size Pooling kernel size.
    /// @param input_offset Defines a shift, relative to (0,0) position of the input buffer, where (0,0) point of the pooling window should start calculations.
    pooling(const primitive_id& id,
            const primitive_id& input,
            pooling_mode mode,
            const tensor& size,
            const tensor& stride,
            const tensor& input_offset = {0, 0, 0, 0},
            const primitive_id& ext_prim_id = "",
            const padding& output_padding = padding())
        : primitive_base(id, {input}, ext_prim_id, output_padding),
          argmax(""),
          mode(static_cast<pooling_mode>(mode)),
          global_pooling(false),
          input_offset(input_offset),
          stride(stride),
          size(size),
          with_output_size(false) {}

    /// @brief Constructs pooling primitive with argmax.
    /// @param id This primitive id.
    /// @param ext_prim_id
    /// @param input Input primitive id.
    /// @param argmax Primitive id which contains indices of each max pooling region.
    /// Indices must be in flattened bfyx format with no padding. Needs to be fp32 data type.
    /// @param mode Pooling mode.
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param size Pooling kernel size.
    /// @param input_offset Defines a shift, relative to (0,0) position of the input buffer,
    /// where (0,0) point of the pooling window should start calculations.
    pooling(const primitive_id& id,
            const primitive_id& input,
            const primitive_id& argmax,
            pooling_mode mode,
            const tensor& size,
            const tensor& stride,
            const tensor& input_offset = {0, 0, 0, 0},
            const primitive_id& ext_prim_id = "",
            const padding& output_padding = padding())
        : primitive_base(id, {input}, ext_prim_id, output_padding),
          argmax(argmax),
          mode(static_cast<pooling_mode>(mode)),
          global_pooling(false),
          input_offset(input_offset),
          stride(stride),
          size(size),
          with_output_size(false) {}

    /// @brief Constructs pooling primitive (computes input paddings to match output size).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param mode Pooling mode.
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param size Pooling kernel size.
    /// @param input_offset Defines a shift, relative to (0,0) position of the input buffer, where (0,0) point of the pooling window should start calculations.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    pooling(const primitive_id& id,
            const primitive_id& input,
            pooling_mode mode,
            const tensor& size,
            const tensor& stride,
            const tensor& input_offset,
            tensor output_size,
            const data_types output_data_type,
            const primitive_id& ext_prim_id = "",
            const padding& output_padding = padding())
        : primitive_base(id, {input}, ext_prim_id, output_padding, optional_data_type{output_data_type}),
          argmax(""),
          mode(static_cast<pooling_mode>(mode)),
          global_pooling(false),
          input_offset(input_offset),
          stride(stride),
          size(size),
          with_output_size(true),
          output_size(output_size) {}

    /// @brief Constructs pooling primitive with argmax (computes input paddings to match output size).
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param argmax Primitive id which contains indices of each max pooling region.
    /// Indices must be in flattened bfyx format with no padding. Needs to be fp32 data type.
    /// @param mode Pooling mode.
    /// @param stride Defines shift in input buffer between adjacent calculations of output values.
    /// @param size Pooling kernel size.
    /// @param input_offset Defines a shift, relative to (0,0) position of the input buffer, where (0,0) point of the pooling window should start calculations.
    /// @param output_size User-defined output data size of the primitive (w/o padding).
    pooling(const primitive_id& id,
            const primitive_id& input,
            const primitive_id& argmax,
            pooling_mode mode,
            const tensor& size,
            const tensor& stride,
            const tensor& input_offset,
            tensor output_size,
            const primitive_id& ext_prim_id = "",
            const padding& output_padding = padding())
        : primitive_base(id, {input}, ext_prim_id, output_padding),
          argmax(argmax),
          mode(static_cast<pooling_mode>(mode)),
          global_pooling(false),
          input_offset(input_offset),
          stride(stride),
          size(size),
          with_output_size(true),
          output_size(output_size) {}

    /// @brief Constructs pooling primitive with kernel size equal to the spatial dimension of input tensor.
    /// @param id This primitive id.
    /// @param input Input primitive id.
    /// @param mode Pooling mode.
    pooling(const primitive_id& id,
            const primitive_id& input,
            pooling_mode mode,
            const primitive_id& ext_prim_id = "",
            const padding& output_padding = padding())
        : primitive_base(id, {input}, ext_prim_id, output_padding),
          argmax(""),
          mode(static_cast<pooling_mode>(mode)),
          global_pooling(true),
          input_offset(0, 0, 0, 0),
          stride(1, 1, 1, 1),
          size(0, 0, 0, 0),
          with_output_size(false) {}

    /// @brief Primitive id which contains indices of each max pooling region.
    /// Indices must be in flattened bfyx format with no padding. Needs to be fp32 data type.
    primitive_id argmax;
    /// @brief Pooling mode.
    pooling_mode mode;
    /// @brief Global pooling (kernel size is equal to the spatial dimension of input tensor)
    bool global_pooling;
    /// @brief Defines a shift, relative to (0,0) position of the input buffer, where (0,0) point of the pooling window should start calculations.
    tensor input_offset;
    /// @brief Defines shift in input buffer between adjacent calculations of output values.
    tensor stride;
    /// @brief Pooling kernel size.
    tensor size;
    /// @brief Indicates that the primitive has user-defined output size (non-zero value).
    bool with_output_size;
    /// @brief User-defined output data size of the primitive (w/o padding).
    tensor output_size;
    /// @brief Defines a shift, relative to the end of padding shape.
    tensor pad_end;

protected:
    std::vector<std::reference_wrapper<const primitive_id>> get_dependencies() const override {
        if (argmax.empty())
            return {};
        return {argmax};
    }
};
/// @}
/// @}
/// @}
}  // namespace cldnn
