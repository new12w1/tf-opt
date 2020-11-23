// Copyright 2020 The tf.opt Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LEARNING_BRAIN_RESEARCH_TF_OPT_SHARED_OPS_RELU_OPERATION_H_
#define LEARNING_BRAIN_RESEARCH_TF_OPT_SHARED_OPS_RELU_OPERATION_H_

#include <string>
#include <vector>

#include "learning/brain/research/tf_opt/shared/graph/operation.h"
#include "learning/brain/research/tf_opt/shared/graph/operation_visitor.h"
#include "absl/status/statusor.h"
#include "tf_opt/neural_net/neural_net.pb.h"
#include "tf_opt/neural_net/neuron/relu_impl_type.h"
#include "tf_opt/tensor/shape.h"

namespace tf_opt {

// Given an input MPTensor or DoubleTensor x, computes the output MPTensor
// y = ReLU(x), where ReLU(a) = max(a, 0) is applied componentwise.
//
// Multiple MIP formulations for y = max(a,0) (where a is a LinearExpr) are
// supported, as specified by setting the string option "formulation":
//   * "Big-M": see relu_big_m.h
//   * "Multiple Choice": see relu_multiple_choice.h
//   * "Multiple Choice Simplified": see relu_multiple_choice_simplified.h
//   * "default": do what the solver thinks is best.
// If "formulation" key is missing, "default" is used automatically.
class ReluOperation : public Operation {
 public:
  static constexpr const char kOptionsFormulationKey[] = "formulation";
  static constexpr const char kOptionsFormulationDefault[] = "default";
  static const char* OptionsFormulationBigM();
  static const char* OptionsFormulationMultipleChoice();
  static const char* OptionsFormulationMultipleChoiceSimplified();

  static absl::StatusOr<ReluOperation> Create(
      std::string op_name, Shape input_shape,
      ReluImplementationType formulation = ReluImplementationType::kBigM);

  static MaybeForGraph<ReluOperation> CreateForGraph(
      std::string op_name, const Operation* input,
      ReluImplementationType formulation = ReluImplementationType::kBigM);

  // Expected input format:
  //   input_shapes: The dimensions of exactly one tensor to transform.
  //   output_shape: The shape of the resulting tensor, same as the inputs[0].
  //   options: May contain a string option with key kOptionsFormulationKey, to
  //       pick a MIP formulation for ReLU().
  static absl::StatusOr<ReluOperation> GenericCreate(
      std::string op_name, std::vector<Shape> input_shapes, Shape output_shape,
      const Options& options);

  const Shape& input() const { return input_shape(0); }
  const ReluImplementationType formulation() const { return formulation_; }

  void Accept(OperationVisitor* visitor) const override {
    visitor->Visit(*this);
  }

  proto::TensorNode ToProto(
      const std::vector<std::string>& inputs) const override;

 private:
  ReluOperation(std::string op_name, Shape input_shape,
                ReluImplementationType formulation);
  // TODO: move this into MIP world.
  ReluImplementationType formulation_;
};

}  // namespace tf_opt

#endif  // LEARNING_BRAIN_RESEARCH_TF_OPT_SHARED_OPS_RELU_OPERATION_H_
