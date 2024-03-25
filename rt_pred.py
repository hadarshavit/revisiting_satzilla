import pandas as pd
import plotly.express as px
import numpy as np
from pyrfr import regression
from pyrfr.regression import binary_rss_forest as BinaryForest
from pyrfr.regression import default_data_container as DataContainer
from ConfigSpace import ConfigurationSpace
from ConfigSpace.hyperparameters import CategoricalHyperparameter, \
    UniformFloatHyperparameter, UniformIntegerHyperparameter, OrdinalHyperparameter
from ConfigSpace.conditions import EqualsCondition, InCondition
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import cross_val_score, KFold
from sklearn.metrics import mean_squared_error
from smac import HyperparameterOptimizationFacade, Scenario
import submitit


features_names = ['nvarsOrig', 'nclausesOrig', 'nvars', 'nclauses', 'reducedVars', 'reducedClauses', 'Pre-featuretime', 'vars-clauses-ratio', 'POSNEG-RATIO-CLAUSE-mean', 'POSNEG-RATIO-CLAUSE-coeff-variation', 'POSNEG-RATIO-CLAUSE-min', 'POSNEG-RATIO-CLAUSE-max', 'POSNEG-RATIO-CLAUSE-entropy', 'VCG-CLAUSE-mean', 'VCG-CLAUSE-coeff-variation', 'VCG-CLAUSE-min', 'VCG-CLAUSE-max', 'VCG-CLAUSE-entropy', 'UNARY', 'BINARY+', 'TRINARY+', 'Basic-featuretime', 'VCG-VAR-mean', 'VCG-VAR-coeff-variation', 'VCG-VAR-min', 'VCG-VAR-max', 'VCG-VAR-entropy', 'POSNEG-RATIO-VAR-mean', 'POSNEG-RATIO-VAR-stdev', 'POSNEG-RATIO-VAR-min', 'POSNEG-RATIO-VAR-max', 'POSNEG-RATIO-VAR-entropy', 'HORNY-VAR-mean', 'HORNY-VAR-coeff-variation', 'HORNY-VAR-min', 'HORNY-VAR-max', 'HORNY-VAR-entropy', 'horn-clauses-fraction', 'VG-mean', 'VG-coeff-variation', 'VG-min', 'VG-max', 'KLB-featuretime', 'CG-mean', 'CG-coeff-variation', 'CG-min', 'CG-max', 'CG-entropy', 'cluster-coeff-mean', 'cluster-coeff-coeff-variation', 'cluster-coeff-min', 'cluster-coeff-max', 'cluster-coeff-entropy', 'CG-featuretime', 'SP-bias-mean', 'SP-bias-coeff-variation', 'SP-bias-min', 'SP-bias-max', 'SP-bias-q90', 'SP-bias-q10', 'SP-bias-q75', 'SP-bias-q25', 'SP-bias-q50', 'SP-unconstraint-mean', 'SP-unconstraint-coeff-variation', 'SP-unconstraint-min', 'SP-unconstraint-max', 'SP-unconstraint-q90', 'SP-unconstraint-q10', 'SP-unconstraint-q75', 'SP-unconstraint-q25', 'SP-unconstraint-q50', 'sp-featuretime', 'DIAMETER-mean', 'DIAMETER-coeff-variation', 'DIAMETER-min', 'DIAMETER-max', 'DIAMETER-entropy', 'DIAMETER-featuretime', 'cl-num-mean', 'cl-num-coeff-variation', 'cl-num-min', 'cl-num-max', 'cl-num-q90', 'cl-num-q10', 'cl-num-q75', 'cl-num-q25', 'cl-num-q50', 'cl-size-mean', 'cl-size-coeff-variation', 'cl-size-min', 'cl-size-max', 'cl-size-q90', 'cl-size-q10', 'cl-size-q75', 'cl-size-q25', 'cl-size-q50', 'cl-featuretime', 'vars-reduced-depth-1', 'vars-reduced-depth-4', 'vars-reduced-depth-16', 'vars-reduced-depth-64', 'vars-reduced-depth-256', 'unit-featuretime', 'saps_BestSolution_Mean', 'saps_BestSolution_CoeffVariance', 'saps_FirstLocalMinStep_Mean', 'saps_FirstLocalMinStep_CoeffVariance', 'saps_FirstLocalMinStep_Median', 'saps_FirstLocalMinStep_Q.10', 'saps_FirstLocalMinStep_Q.90', 'saps_BestAvgImprovement_Mean', 'saps_BestAvgImprovement_CoeffVariance', 'saps_FirstLocalMinRatio_Mean', 'saps_FirstLocalMinRatio_CoeffVariance', 'ls-saps-featuretime', 'gsat_BestSolution_Mean', 'gsat_BestSolution_CoeffVariance', 'gsat_FirstLocalMinStep_Mean', 'gsat_FirstLocalMinStep_CoeffVariance', 'gsat_FirstLocalMinStep_Median', 'gsat_FirstLocalMinStep_Q.10', 'gsat_FirstLocalMinStep_Q.90', 'gsat_BestAvgImprovement_Mean', 'gsat_BestAvgImprovement_CoeffVariance', 'gsat_FirstLocalMinRatio_Mean', 'gsat_FirstLocalMinRatio_CoeffVariance', 'ls-gsat-featuretime', 'lobjois-mean-depth-over-vars', 'lobjois-log-num-nodes-over-vars', 'lobjois-featuretime']

sc2022_solvers = ['CaDiCaL-watchsat-lto',
 'CaDiCaL_DVDL_V1',
 'CaDiCaL_DVDL_V2',
 'CadicalReorder',
 'Cadical_ESA',
 'IsaSAT',
 'Kissat-MAB-rephasing',
 'Kissat_MAB-HyWalk',
 'Kissat_MAB_ESA',
 'Kissat_MAB_MOSS',
 'Kissat_MAB_UCB',
 'Kissat_adaptive_restart',
 'Kissat_cfexp',
 'LSTech_CaDiCaL',
 'LSTech_Maple',
 'LSTech_kissat',
 'LStech-Maple-BandSAT',
 'LStech-Maple-FPS',
 'LStech-Maple-HyWalk',
 'MapleLCMDistChrBt-DL-v3',
 'MergeSat 4.0-rc-rc3',
 'SLIME SC-2022',
 'SLIME SC-2022-alpha',
 'SLIME SC-2022-beta',
 'SLIME SC-2022-gamma',
 'SeqFROST-ERE-All',
 'SeqFROST-NoExtend',
 'cadical-hack-gb',
 'cadical_rel_Scavel',
 'ekissat-mab-db-v1',
 'ekissat-mab-db-v2',
 'ekissat-mab-gb-db',
 'glucose-reboot',
 'hCaD_V1-psids',
 'hCaD_V2',
 'hKis-psids',
 'hKis-sat',
 'hKis-unsat',
 'kissat-els-v1',
 'kissat-els-v2',
 'kissat-els-v3',
 'kissat-els-v4',
 'kissat-mab-gb',
 'kissat-sc2022-bulky',
 'kissat-sc2022-hyper',
 'kissat-sc2022-light',
 'kissat-watchsat-lto',
 'kissat_inc',
 'kissat_pre',
 'kissat_relaxed']

sc2023_solvers = ['AMSAT_',
 'BreakID-kissat-low.sh',
 'CaDiCaL_vivinst',
 'Cadical_ESA',
 'Cadical_rel_1.5.3.Scavel',
 'IsaSAT',
 'Kissat_Inc_ESA',
 'Kissat_MAB_Binary',
 'Kissat_MAB_Conflict',
 'Kissat_MAB_Conflict+',
 'Kissat_MAB_DeepWalk+',
 'Kissat_MAB_ESA',
 'Kissat_MAB_Rephases',
 'Kissat_MAB_prop',
 'Kissat_MAB_prop-no_sym',
 'Kissat_MAB_prop_pr-no_sym',
 'MapleCaDiCaL_LBD-990_275',
 'MapleCaDiCaL_LBD-990_500',
 'MapleCaDiCaL_PPD-500_500',
 'MapleCaDiCaL_PPD-950_950',
 'MergeSat-bve_gates',
 'MergeSat-bve_semgates',
 'MergeSat-thread1',
 'MiniSat+XorEngine',
 'PReLearn-kissat-PReLearn-kissat.sh',
 'PReLearn-kissat-PReLearn-tern-kissat.sh',
 'ReEncode-kissat-ReEncode-pair-kissat.sh',
 'SBVA-sbva_cadical',
 'SBVA-sbva_kissat',
 'SeqFROST',
 'SeqFROST-ERE-All',
 'SeqFROST-NoExtend',
 'hKis-psids',
 'hKis-sat_psids',
 'hKis-unsat',
 'hKissatInc-unsat',
 'kissat-3.1.0',
 'kissat-hywalk-exp',
 'kissat-hywalk-exp-gb',
 'kissat-hywalk-gb',
 'kissat_incsp',
 'tabularasat-1.0.0']

def _init_data_container(X: np.ndarray, y: np.ndarray) -> DataContainer:
    """Fills a pyrfr default data container s.t. the forest knows categoricals and bounds for continous data.

    Parameters
    ----------
    X : np.ndarray [#samples, #hyperparameter + #features]
        Input data points.
    Y : np.ndarray [#samples, #objectives]
        The corresponding target values.

    Returns
    -------
    data : DataContainer
        The filled data container that pyrfr can interpret.
    """
    # Retrieve the types and the bounds from the ConfigSpace
    data = regression.default_data_container(X.shape[1])

    for col in range(X.shape[1]):
        data.set_bounds_of_feature(col, X[:, col].min(), X[:, col].max())

    for row_X, row_y in zip(X, y.reshape(-1, 1)):
        data.add_data_point(row_X, row_y)

    return data
    

def execute(data_file, solver, smac_name, train_index, test_index):
    df = pd.read_csv(data_file, index_col=0)
    X = df[features_names].values
    Y = df[solver].values
    Xtrain = X[train_index]
    Xtest = X[test_index]
    Ytrain = np.log10(Y[train_index])
    Ytest = np.log10(Y[test_index])

    cs = ConfigurationSpace()

    n_estimators = UniformIntegerHyperparameter(
                    name="rfreg:n_estimators", lower=10, upper=100, default_value=10, log=True)
    cs.add_hyperparameter(n_estimators)
    max_features = OrdinalHyperparameter(
        name="rfreg:max_features", sequence=[0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0], default_value=1.0)
    cs.add_hyperparameter(max_features)
    max_depth = UniformIntegerHyperparameter(
        name="rfreg:max_depth", lower=10, upper=2 ** 31, default_value=2 ** 31, log=True)
    cs.add_hyperparameter(max_depth)
    min_samples_split = UniformIntegerHyperparameter(
        name="rfreg:min_samples_split", lower=2, upper=100, default_value=2, log=True)
    cs.add_hyperparameter(min_samples_split)
    min_samples_leaf = UniformIntegerHyperparameter(
        name="rfreg:min_samples_leaf", lower=2, upper=100, default_value=10, log=True)
    cs.add_hyperparameter(min_samples_leaf)
    bootstrap = CategoricalHyperparameter(
        name="rfreg:bootstrap", choices=[True, False], default_value=True)
    cs.add_hyperparameter(bootstrap)

    def train(config, seed):
        model = RandomForestRegressor(n_estimators=config["rfreg:n_estimators"],
                                            max_features=config[
                                                "rfreg:max_features"] if config[
                                                "rfreg:max_features"] != "None" else None,
                                            max_depth=config["rfreg:max_depth"],
                                            min_samples_split=config[
                                                "rfreg:min_samples_split"],
                                            min_samples_leaf=config[
                                                "rfreg:min_samples_leaf"],
                                            bootstrap=config["rfreg:bootstrap"],
                                            random_state=12345)
        
        return -np.mean(cross_val_score(model, Xtrain, Ytrain, scoring='neg_mean_squared_error', cv=KFold(n_splits=10, shuffle=True, random_state=42)))


    s = Scenario(
        configspace=cs,
        output_directory=f'smac_out/{smac_name}',
        deterministic=True,
        walltime_limit=3600,
        n_trials=1024*1024,
        seed=0
    )

    smac = HyperparameterOptimizationFacade(scenario=s, target_function=train)
    inc = smac.optimize()

    model = RandomForestRegressor(n_estimators=inc["rfreg:n_estimators"],
                                            max_features=inc[
                                                "rfreg:max_features"] if inc[
                                                "rfreg:max_features"] != "None" else None,
                                            max_depth=inc["rfreg:max_depth"],
                                            min_samples_split=inc[
                                                "rfreg:min_samples_split"],
                                            min_samples_leaf=inc[
                                                "rfreg:min_samples_leaf"],
                                            bootstrap=inc["rfreg:bootstrap"],
                                            random_state=12345)
    model.fit(Xtrain, Ytrain)
    preds = model.predict(Xtest)
    mse = mean_squared_error(Ytest, preds)
    np.save(f'rts_results/{smac_name}', np.array([model, preds, mse], dtype=object))

if __name__ == '__main__':
    executor = submitit.AutoExecutor('logs', 'slurm')
    executor.update_parameters(timeout_min=3600, slurm_partition="Kathleen", slurm_array_parallelism=1024, cpus_per_task=1, mem_gb=4, slurm_additional_parameters={'exclude': 'kathleencpu[01-05]'}, job_name='rt_pred')
    for scenario in ['sc2022', 'sc2023']:
        solvers_list = sc2022_solvers if scenario == 'sc2022' else sc2023_solvers
        with executor.batch():
            for solver in solvers_list:
                for old_ending in ['', '_old']:
                    data_file = f'data/{scenario}{old_ending}.csv'
                    folds = np.load(f'data/{scenario}_folds.npy', allow_pickle=True)
                    for fold in range(10):
                        smac_name = f'{scenario}{old_ending}_{solver}_{fold}'
                        train_index, test_index = folds[fold]
                        j = executor.submit(execute, data_file, solver, smac_name, train_index, test_index)
