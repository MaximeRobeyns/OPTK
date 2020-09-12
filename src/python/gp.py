"""
Copyright (C) 2020 Maxime Robeyns <maximerobeyns@gmail.com>

Written for the ACRC, University of Bristol

Licensed under the Educational Community License, Version 2.0
(the "License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at

http://www.osedu.org/licenses/ECL-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the LIcense For The Specific Language Governing permissions and
limitations under the License.
"""

"""
Warning: the contents of this file are deprecated and it is kept for reference.
"""

import numpy as np
import sys
from typing import Callable, List
from scipy.optimize import minimize
from scipy.stats import norm
from sklearn.gaussian_process import GaussianProcessRegressor
from sklearn.gaussian_process.kernels import ConstantKernel, Matern

def expected_improvement(
        X: np.ndarray,
        X_obs: np.ndarray,
        Y_obs: np.ndarray,
        gpr: GaussianProcessRegressor,
        xi:float = 0.01
        ) -> np.ndarray:
    """
    Calculates the expected improvement (EI) at the range of points X, using the
    observed locations X_obs and corresponding outputs Y_obs to build the
    response surface using a Gaussian process.

    Args:
        X: The test locations at which to evaluate the EI (m x d)
        X_obs: The observed input locations (n x d)
        Y_obs: The observed scalar response values (n x 1)
        gpr: A sklearn.gaussian_process.GaussianProcessRegressor
        xi: The exploration/exploitation tradeoff parameter

    Returns:
        The expected improvements at test locations X
    """
    mu, sigma = gpr.predict (X, return_std=True)
    mu_sample = gpr.predict (X_obs)

    sigma = sigma.reshape (-1, 1)

    mu_sample_opt = np.max(mu_sample)

    with np.errstate(divide='warn'):
        imp = mu - mu_sample_opt - xi
        Z = imp / sigma
        ei = imp * norm.cdf(Z) + sigma * norm.pdf(Z)
        ei[sigma == 0.0] = 0.0

    return ei

def generate_parameters (
        acquisition: Callable,
        X_obs: List[List[float]],
        Y_obs: List[float],
        bounds: List[List[float]]
        ) -> np.ndarray:
    """
    This function generates a new test location by optimising the acquisition
    function over the Gaussian process provided with previous samples.

    Args:
        acquisition: The acquisition function to use
        X_obs: The observed input locations (n x d)
        Y_obs: The corresponding observed scalar outputs (n x 1)
        bounds: An array of bounds on the search space

    Returns:
        An array of the next suggested test locations which maximise the
        acquisition function.
    """
    Xs = np.array(X_obs)
    Ys = np.array(Y_obs)
    bs = np.array(bounds)

    # return a random point at first:
    if (Xs.size == 0):
        return np.random.uniform(bs[:,0], bs[:, 1], size=bs.shape[0]).tolist()

    noise = 0.2
    m52 = ConstantKernel(1.0) * Matern(length_scale=1.0, nu=2.5)
    gpr = GaussianProcessRegressor(kernel=m52, alpha=noise**2)

    n_restarts = 25

    gpr.fit (Xs, Ys)

    dim = Xs.shape[1]
    min_val = sys.float_info.max
    min_x = None

    def min_obj(X):
        return -acquisition(X.reshape(-1, dim), Xs, Ys, gpr)

    for x0 in np.random.uniform(bs[:, 0], bs[:, 1], size=(n_restarts, dim)):
        res = minimize(min_obj, x0=x0, bounds=bs, method='L-BFGS-B')
        if res.fun < min_val:
            min_val = res.fun[0]
            min_x = res.x

    return min_x.tolist()
