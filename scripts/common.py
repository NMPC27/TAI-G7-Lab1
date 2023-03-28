from typing import Tuple

def parse_processed_name(name: str) -> Tuple[str, int, float, str, str, int]:
    parameters = name.split('_')
    fname_parameter_span = len(parameters) - 5

    fname = '_'.join(parameters[:fname_parameter_span])
    k = int(parameters[fname_parameter_span])
    alpha = float(parameters[fname_parameter_span + 1])
    probability_distribution = parameters[fname_parameter_span + 2]
    pointer_manager = parameters[fname_parameter_span + 3]
    thresholds = parameters[fname_parameter_span + 4]

    return fname, k, alpha, probability_distribution, pointer_manager, thresholds