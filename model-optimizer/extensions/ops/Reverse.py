# Copyright (C) 2018-2021 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

import numpy as np

from mo.front.common.partial_infer.utils import int64_array
from mo.graph.graph import Graph
from mo.ops.op import Op


class Reverse(Op):
    op = 'Reverse'

    def __init__(self, graph: Graph, attrs: dict):
        mandatory_props = {
            'type': None,
            'axis': None,
            'op': self.op,
            'in_ports_count': 2,
            'out_ports_count': 1,
            'infer': self.infer,
        }
        super().__init__(graph, mandatory_props, attrs)

    @staticmethod
    def infer(node):
        input_shape = node.in_port(0).data.get_shape()
        input_value = node.in_port(0).data.get_value()
        assert input_shape is not None
        if not node.has_valid('axis'):
            assert 1 in node.in_nodes()
            assert node.in_node(1).has_valid('value')
            assert node.in_node(1).value.size == 1

            node['axis'] = node.in_node(1).value.item()
            node.in_port(1).disconnect()

        assert node.has_valid('axis')

        assert len(node.out_nodes()) == 1
        if input_value is not None:
            node.out_port(0).data.set_value(np.flip(input_value, node.axis))
        else:
            node.out_port(0).data.set_shape(input_shape)
