#!/usr/bin/env python
# coding: utf-8
# author: Nikolay Karpushkin
# example: python3 centrality_clusterization.py -p /home/nikolay/BMN/tmp/7988_shortTreeVtx_mpd_run_Top.root -f fFHCal,fHodo -b 500 -n 10 -e 0.01 -m results/model.pkl

import argparse
import uproot
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

def load_data(input_root_file, feature_names):
    # Load data from the ROOT file
    events = uproot.open(input_root_file+":bmndata")
    selected_events = {}
    # selected_events = {
    #     "Trigger": events["BmnTrigInfo./BmnTrigInfo.fInputsAR"].array(library="np"),
    #     "BC1S": events["BmnTrigInfo./BmnTrigInfo.fBC1Integral"].array(library="np"),
    #     "VertexZ": events["PrimaryVertex./PrimaryVertex.fZ"].array(library="np"),
    #     "VertexNTracks": events["PrimaryVertex./PrimaryVertex.fNTracks"].array(library="np")
    # }
    for feat_name in feature_names:
        selected_events[feat_name] = events[feat_name].array(library="np")
    return pd.DataFrame(selected_events)

def apply_selection(df):
    # # Extracting individual bits
    # bitBT = (df["Trigger"].apply(lambda x: (x >> 2) & 1)).astype(bool)
    # bitMBT = (df["Trigger"].apply(lambda x: (x >> 5) & 1)).astype(bool)
    # bitNiT = (df["Trigger"].apply(lambda x: (x >> 3) & 1)).astype(bool)
    # bitCCT1 = (df["Trigger"].apply(lambda x: (x >> 4) & 1)).astype(bool)
    # bitCCT2 = (df["Trigger"].apply(lambda x: (x >> 7) & 1)).astype(bool)

    # # Forming boolean conditions
    # trigBT = (bitBT == 1) & (bitMBT == 0) & (bitCCT1 == 0) & (bitCCT2 == 0)
    # trigMBT = (bitMBT == 1) & (bitNiT == 0)
    # trigCCT1 = (bitCCT1 == 1) & (bitMBT == 0) & (bitCCT2 == 0)
    # trigCCT2 = (bitCCT2 == 1)

    # # Applying selection based on boolean conditions
    # df = df[trigMBT]
    # df = df[
    #     (df["BC1S"] >= 14000) & (df["BC1S"] <= 40000) &  # BC1S integral
    #     (abs(df["VertexZ"]) <= 1.5) & (df["VertexNTracks"] > 1)  # Vertex conditions
    # ]
    return df.reset_index(drop=True)

#################################################################################################################

def compute_correlation_matrix(df, feature_names, nbins):
    # Define the number of features
    combinations = int(np.math.factorial(len(feature_names)) / (2 * np.math.factorial(len(feature_names) - 2)))

    # Create a grid of subplots
    fig, axes = plt.subplots(1, 1 + combinations, figsize=(8 * (1 + combinations), 8))

    # Compute the correlation matrix
    corr_matrix = df[feature_names].corr()
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', fmt=".2f", linewidths=0.5, ax=axes[0])
    axes[0].set_title('Feature Correlation Matrix')

    counter = 1
    for i, feature1 in enumerate(feature_names):
        for j, feature2 in enumerate(feature_names):
            if j <= i:
                continue
            ax = axes[counter]
            ax.hist2d(df[feature1], df[feature2], bins=(nbins, nbins), cmap='jet', cmin=1, norm="log")
            ax.set_xlabel(feature1)
            ax.set_ylabel(feature2)
            cbar = plt.colorbar(ax.collections[0], ax=ax)
            cbar.set_label('Events', rotation=90)
            counter += 1

    plt.savefig('results/correlations.png')

def normalize_features(df, feature_names):
    # Determine the maximum value for each feature
    max_values = df.max()
    podgon = 1.7
    max_values['fFHCal'] *= podgon

    # Normalize each feature
    for feat_name in feature_names:
        df[f'normalized_{feat_name}'] = df[feat_name] / max_values[feat_name]
    return df, max_values

def set_plt_options():
    # Set the default behavior for matplotlib
    plt.rcParams['savefig.transparent'] = True
    plt.rcParams['font.size'] = 15
    plt.rcParams['figure.subplot.wspace'] = 0.3

def plot_clusters(feature_names, clf, X, max_values):
    # Define the number of pairwise combinations
    combinations = int(np.math.factorial(len(feature_names)) / (2 * np.math.factorial(len(feature_names) - 2)))

    # Create a grid of subplots
    fig, axes = plt.subplots(1, 1 + combinations, figsize=(8 * (1 + combinations), 8))

    # Compute the correlation matrix
    fractions = [np.sum(clf.labels_ == k)/len(X) for k in range(clf.n_clusters)]
    colors = [plt.cm.viridis(i) for i in np.linspace(0, 1, clf.n_clusters)]
    counts = axes[0].bar(range(clf.n_clusters), fractions, color=colors)
    axes[0].set_xlabel('Cluster')
    axes[0].set_ylabel('Fraction of events')

    counter = 1
    for i, feature1 in enumerate(feature_names):
        for j, feature2 in enumerate(feature_names):
            if j <= i:
                continue
            ax = axes[counter]
            ax.scatter(max_values[feature1] * X[:, i], max_values[feature2] * X[:, j], c=clf.labels_, s=0.1, cmap='viridis')
            ax.scatter(max_values[feature1] * clf.cluster_centers_[:, i],max_values[feature2]*clf.cluster_centers_[:, j], marker='x', color='red', s=100)
            ax.set_xlabel(feature1)
            ax.set_ylabel(feature2)
            counter += 1

    plt.savefig('results/clusters.png')


def create_pdfs(feature_names, clf, df, nbins):
    from ROOT import TCanvas, gPad, TH1F, TFile

    # Create canvas
    c1 = TCanvas('c1', 'Probability density functions', 800*clf.n_clusters, 800*len(feature_names))
    c1.Divide(clf.n_clusters, len(feature_names))

    # Dictionary to store histograms
    histos = {}

    # Loop over each feature and cluster to create histograms
    for feat_name in feature_names:
        for cluster in range(clf.n_clusters):
            hist_name = f"cluster_{cluster}/{feat_name}"
            hist_title = hist_name
            hist = TH1F(hist_name, hist_title, nbins, 0, df[feat_name].max())
            histos[hist_name] = hist

    # Fill histograms with data from DataFrame
    for event in range(len(df)):
        cluster = clf.labels_[event]
        for feat_name in feature_names:
            hist_name = f"cluster_{cluster}/{feat_name}"
            hist = histos[hist_name]
            hist.Fill(df[feat_name][event])

    # Draw histograms on canvas
    counter = 0
    for feat_name in feature_names:
        for cluster in range(clf.n_clusters):
            c1.cd(counter+1)
            gPad.SetLogy()
            hist_name = f"cluster_{cluster}/{feat_name}"
            hist = histos[hist_name]
            hist.Scale(1./hist.Integral())
            hist.Draw('N hist')
            counter += 1

    # Save canvas
    c1.Draw()
    c1.SaveAs("results/feature_pdfs.png")
    c1.SaveAs("results/feature_pdfs_cc.root")

    root_file = TFile("results/feature_pdfs.root", "RECREATE")
    for hist_name in histos:
        hist = histos[hist_name]
        hist.Write()
    root_file.Close()


def main(input_root_file, feature_names, nbins, n_clusters, eps, load_model):
    # Set the default behavior for matplotlib
    set_plt_options()

    df = load_data(input_root_file, feature_names)
    df = apply_selection(df)
    compute_correlation_matrix(df, feature_names, nbins)

    # Normalize features
    df, max_values = normalize_features(df, feature_names)
    X = df[[f'normalized_{feat_name}' for feat_name in feature_names]].values
    
    if load_model == "None":
        from k_means_constrained import KMeansConstrained
        import pickle
        
        clf = KMeansConstrained(n_clusters=n_clusters, size_min=int(len(X)/n_clusters), size_max=int((1.0+eps)*len(X)/n_clusters), n_jobs=-1)
        clf.fit_predict(X)

        with open("results/model.pkl", "wb") as f:
            pickle.dump(clf, f)
    else:
        import pickle
        with open(load_model, "rb") as f:
            clf = pickle.load(f)

    # Create a grid of subplots
    plot_clusters(feature_names, clf, X, max_values)

    # Create probability density functions
    create_pdfs(feature_names, clf, df, nbins)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Python script to clusterize data by their features into clusters of equal size")
    parser.add_argument("-p", "--path", required=True, help="Path to the BMNROOT DST file")
    parser.add_argument("-f", "--features", required=True, help="Feature names separated by commas (e.g. FHCalEvent/fTotalEnergy, HodoEvent/fTotalSignal)")
    parser.add_argument("-b", "--nbins", type=int, required=True, help="Number of bins for histograms")
    parser.add_argument("-n", "--nclusters", type=int, required=True, help="Number of clusters")
    parser.add_argument("-e", "--eps", type=float, default=0.01, help="Value of fluctuation of statistics in clusters")
    parser.add_argument("-m", "--model", default="None", help="Path to the pre-trained model")

    args = parser.parse_args()
    feature_names = args.features.split(",")

    main(args.path, feature_names, args.nbins, args.nclusters, args.eps, args.model)