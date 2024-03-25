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
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import cross_val_score, KFold
from sklearn.metrics import f1_score, accuracy_score
from smac import HyperparameterOptimizationFacade, Scenario
import submitit


features_names = ['nvarsOrig', 'nclausesOrig', 'nvars', 'nclauses', 'reducedVars', 'reducedClauses', 'Pre-featuretime', 'vars-clauses-ratio', 'POSNEG-RATIO-CLAUSE-mean', 'POSNEG-RATIO-CLAUSE-coeff-variation', 'POSNEG-RATIO-CLAUSE-min', 'POSNEG-RATIO-CLAUSE-max', 'POSNEG-RATIO-CLAUSE-entropy', 'VCG-CLAUSE-mean', 'VCG-CLAUSE-coeff-variation', 'VCG-CLAUSE-min', 'VCG-CLAUSE-max', 'VCG-CLAUSE-entropy', 'UNARY', 'BINARY+', 'TRINARY+', 'Basic-featuretime', 'VCG-VAR-mean', 'VCG-VAR-coeff-variation', 'VCG-VAR-min', 'VCG-VAR-max', 'VCG-VAR-entropy', 'POSNEG-RATIO-VAR-mean', 'POSNEG-RATIO-VAR-stdev', 'POSNEG-RATIO-VAR-min', 'POSNEG-RATIO-VAR-max', 'POSNEG-RATIO-VAR-entropy', 'HORNY-VAR-mean', 'HORNY-VAR-coeff-variation', 'HORNY-VAR-min', 'HORNY-VAR-max', 'HORNY-VAR-entropy', 'horn-clauses-fraction', 'VG-mean', 'VG-coeff-variation', 'VG-min', 'VG-max', 'KLB-featuretime', 'CG-mean', 'CG-coeff-variation', 'CG-min', 'CG-max', 'CG-entropy', 'cluster-coeff-mean', 'cluster-coeff-coeff-variation', 'cluster-coeff-min', 'cluster-coeff-max', 'cluster-coeff-entropy', 'CG-featuretime', 'SP-bias-mean', 'SP-bias-coeff-variation', 'SP-bias-min', 'SP-bias-max', 'SP-bias-q90', 'SP-bias-q10', 'SP-bias-q75', 'SP-bias-q25', 'SP-bias-q50', 'SP-unconstraint-mean', 'SP-unconstraint-coeff-variation', 'SP-unconstraint-min', 'SP-unconstraint-max', 'SP-unconstraint-q90', 'SP-unconstraint-q10', 'SP-unconstraint-q75', 'SP-unconstraint-q25', 'SP-unconstraint-q50', 'sp-featuretime', 'DIAMETER-mean', 'DIAMETER-coeff-variation', 'DIAMETER-min', 'DIAMETER-max', 'DIAMETER-entropy', 'DIAMETER-featuretime', 'cl-num-mean', 'cl-num-coeff-variation', 'cl-num-min', 'cl-num-max', 'cl-num-q90', 'cl-num-q10', 'cl-num-q75', 'cl-num-q25', 'cl-num-q50', 'cl-size-mean', 'cl-size-coeff-variation', 'cl-size-min', 'cl-size-max', 'cl-size-q90', 'cl-size-q10', 'cl-size-q75', 'cl-size-q25', 'cl-size-q50', 'cl-featuretime', 'vars-reduced-depth-1', 'vars-reduced-depth-4', 'vars-reduced-depth-16', 'vars-reduced-depth-64', 'vars-reduced-depth-256', 'unit-featuretime', 'saps_BestSolution_Mean', 'saps_BestSolution_CoeffVariance', 'saps_FirstLocalMinStep_Mean', 'saps_FirstLocalMinStep_CoeffVariance', 'saps_FirstLocalMinStep_Median', 'saps_FirstLocalMinStep_Q.10', 'saps_FirstLocalMinStep_Q.90', 'saps_BestAvgImprovement_Mean', 'saps_BestAvgImprovement_CoeffVariance', 'saps_FirstLocalMinRatio_Mean', 'saps_FirstLocalMinRatio_CoeffVariance', 'ls-saps-featuretime', 'gsat_BestSolution_Mean', 'gsat_BestSolution_CoeffVariance', 'gsat_FirstLocalMinStep_Mean', 'gsat_FirstLocalMinStep_CoeffVariance', 'gsat_FirstLocalMinStep_Median', 'gsat_FirstLocalMinStep_Q.10', 'gsat_FirstLocalMinStep_Q.90', 'gsat_BestAvgImprovement_Mean', 'gsat_BestAvgImprovement_CoeffVariance', 'gsat_FirstLocalMinRatio_Mean', 'gsat_FirstLocalMinRatio_CoeffVariance', 'ls-gsat-featuretime', 'lobjois-mean-depth-over-vars', 'lobjois-log-num-nodes-over-vars', 'lobjois-featuretime']


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
    

def execute(data_file, smac_name, train_index, test_index):
    df = pd.read_csv(data_file, index_col=0)
    df = df[df['vresult'] != 'unknown']
    X = df[features_names].values
    Y = df['vresult'].values
    Xtrain = X[train_index]
    Xtest = X[test_index]
    Ytrain = Y[train_index]
    Ytest = Y[test_index]

    cs = ConfigurationSpace()

    n_estimators = UniformIntegerHyperparameter(
                name="rf:n_estimators", lower=10, upper=100, default_value=10, log=True)
    cs.add_hyperparameter(n_estimators)
    criterion = CategoricalHyperparameter(
        name="rf:criterion", choices=["gini", "entropy"], default_value="gini")
    cs.add_hyperparameter(criterion)
    max_features = OrdinalHyperparameter(
        name="rf:max_features", sequence=[0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0], default_value=1.0)
    cs.add_hyperparameter(max_features)
    max_depth = UniformIntegerHyperparameter(
        name="rf:max_depth", lower=10, upper=2**31, default_value=2**31, log=True)
    cs.add_hyperparameter(max_depth)
    min_samples_split = UniformIntegerHyperparameter(
        name="rf:min_samples_split", lower=2, upper=100, default_value=2, log=True)
    cs.add_hyperparameter(min_samples_split)
    min_samples_leaf = UniformIntegerHyperparameter(
        name="rf:min_samples_leaf", lower=2, upper=100, default_value=10, log=True)
    cs.add_hyperparameter(min_samples_leaf)
    bootstrap = CategoricalHyperparameter(
        name="rf:bootstrap", choices=[True, False], default_value=True)
    cs.add_hyperparameter(bootstrap)

    def train(config, seed):
        model = RandomForestClassifier(n_estimators=config["rf:n_estimators"],
                                            max_features= config[
                                                "rf:max_features"] if config[
                                                "rf:max_features"] != "None" else None,
                                            criterion=config["rf:criterion"],
                                            max_depth=config["rf:max_depth"],
                                            min_samples_split=config[
                                                "rf:min_samples_split"],
                                            min_samples_leaf=config[
                                                "rf:min_samples_leaf"],
                                            bootstrap=config["rf:bootstrap"],
                                            random_state=12345)
        
        return 1 - np.mean(cross_val_score(model, Xtrain, Ytrain, scoring='accuracy', cv=KFold(n_splits=10, shuffle=True, random_state=42)))


    s = Scenario(
        configspace=cs,
        output_directory=f'sat_smac_out/{smac_name}',
        deterministic=True,
        walltime_limit=3600,
        n_trials=1024*1024,
        seed=0
    )

    smac = HyperparameterOptimizationFacade(scenario=s, target_function=train)
    inc = smac.optimize()

    model = RandomForestClassifier(n_estimators=inc["rf:n_estimators"],
                                            max_features= inc[
                                                "rf:max_features"] if inc[
                                                "rf:max_features"] != "None" else None,
                                            criterion=inc["rf:criterion"],
                                            max_depth=inc["rf:max_depth"],
                                            min_samples_split=inc[
                                                "rf:min_samples_split"],
                                            min_samples_leaf=inc[
                                                "rf:min_samples_leaf"],
                                            bootstrap=inc["rf:bootstrap"],
                                            random_state=12345)
    model.fit(Xtrain, Ytrain)
    preds = model.predict(Xtest)
    mse =accuracy_score(Ytest, preds)
    np.save(f'sat_results/{smac_name}', np.array([model, preds, mse], dtype=object))

if __name__ == '__main__':
    executor = submitit.AutoExecutor('logs', 'slurm')
    executor.update_parameters(timeout_min=3600, slurm_partition="Kathleen", slurm_array_parallelism=1024, cpus_per_task=1, mem_gb=4, job_name='sat_pred')
    for scenario in ['sc2022', 'sc2023']:
        with executor.batch():
            for old_ending in ['', '_old']:
                data_file = f'data/{scenario}{old_ending}_sat.csv'
                folds = np.load(f'data/{scenario}_folds_sat.npy', allow_pickle=True)
                for fold in range(10):
                    smac_name = f'{scenario}{old_ending}_{fold}_sat'
                    train_index, test_index = folds[fold]
                    j = executor.submit(execute, data_file, smac_name, train_index, test_index)
                    # j.result()
