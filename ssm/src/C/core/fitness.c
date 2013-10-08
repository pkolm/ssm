/**************************************************************************
 *    This file is part of ssm.
 *
 *    ssm is free software: you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published
 *    by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    ssm is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public
 *    License along with ssm.  If not, see
 *    <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "ssm.h"


/**
 *  checks for numerical issues and avoid 0.0 to avoid NaN when taking
 *  log.  Also, ensures a uniform likelihood scale by making sure that
 *  everything below fitness->like_min is fitness->like_min.
 */
double ssm_sanitize_log_likelihood(double log_like, ssm_row_t *row, ssm_fitness_t *fitness, ssm_nav_t *nav)
{
    if ((isinf(log_like)==1) || (isnan(log_like)==1) || (log_like<0.0) ) { //error
        if (!(nav->print & SSM_QUIET)) {
            char str[SSM_STR_BUFFSIZE];
            sprintf(str, "error likelihood computation, log_like = %g", log_like);
            ssm_print_warning(str);
        }
        return fitness->log_like_min * row->ts_nonan_length;
    } else {
        return GSL_MAX(fitness->log_like_min * row->ts_nonan_length, log_like);
    }
}


double ssm_log_likelihood(ssm_row_t *row, ssm_X_t *X, ssm_par_t *par, ssm_calc_t *calc, ssm_nav_t *nav, ssm_fitness_t *fitness)
{
    int i;
    double loglike = 0.0;
    double t = row->time;

    for(i=0; i< row->ts_nonan_length; i++){
        loglike += log(row->observed[i]->f_likelihood(row->values[i], X, par, calc, t));
    }

    return ssm_sanitize_log_likelihood(loglike, row, fitness, nav);
}


double ssm_sum_square(ssm_row_t *row, ssm_X_t *X, ssm_par_t *par, ssm_calc_t *calc, ssm_nav_t *nav, ssm_fitness_t *fitness)
{
    int i;
    double ss = 0.0;
    double t = row->time;

    for(i=0; i< row->ts_nonan_length; i++){
        ss += pow( row->values[i] - row->observed[i]->f_obs_mean(X, par, calc, t), 2 );
    }

    return ss;
}
