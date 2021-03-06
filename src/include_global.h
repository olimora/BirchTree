#ifndef CONFIG_H
#define CONFIG_H

#ifdef _MSC_VER
#pragma once
#endif


#include "include_basic.h"
#include "class_CFTree.h"
#include "algo_distance.h"
#include "algo_calculations.h"
#include <string>

class Global {
private:
    Global() {};
    Global(const Global&);
    Global& operator=(const Global&);

    float memory_without_tree;
	float memory_max = 0;

    int dimensions;
    int BF_B;
    int BF_L;
    double threshold_T;

    int memory_limit_MB;
    int subcluster_limit;
    double rebuild_size_factor;

    bool remove_outliers;
    int rebuild_count_before_outlier_removal;

	distanceFPtr distanceFunc;
    clusterSizeFPtr clusterSizeFunc;

	std::shared_ptr<CFTree> tree;
//    std::shared_ptr<CFTree> new_tree;

public:
	static Global& get()
	{
		static Global instance;
		return instance;
	}

    double trhs_dist_sum;
    int trhs_dist_count;
    int rebuild_count;
    std::vector<std::shared_ptr<CF>> outliers;

	float getMemoryWithoutTree() { return this->memory_without_tree; }
	void setMemoryWithoutTree(float mem) { this->memory_without_tree = mem;  }

	float getMemoryMax() { return this->memory_max; }
	void setMemoryMax(float mem) {
		if (mem > this->memory_max) {
			this->memory_max = mem;
		}
	}

	int getDimensions() { return this->dimensions; }
	void setDimensions(int dims) { this->dimensions = dims;  }

    int getBF_B() { return this->BF_B; }
    void setBF_B(int BF_B) { this->BF_B = BF_B;  }

    int getBF_L() { return this->BF_L; }
    void setBF_L(int BF_L) { this->BF_L = BF_L;  }

    double getThreshold_T() { return this->threshold_T; }
    void setThreshold_T(double t) { this->threshold_T = t;  }

    int getMemoryLimitMB() { return this->memory_limit_MB; }
    void setMemoryLimitMB(int memory_limit_MB) { this->memory_limit_MB = memory_limit_MB;  }

    int getSubclusterLimit() { return this->subcluster_limit; }
    void setSubclusterLimit(int subcluster_limit) { this->subcluster_limit = subcluster_limit;  }

    double getRebuildSizeFactor() { return this->rebuild_size_factor; }
    void setRebuildSizeFactor(double rebuild_size_factor) { this->rebuild_size_factor = rebuild_size_factor;  }

	bool getRemoveOutliers() { return this->remove_outliers; }
	void setRemoveOutliers(bool remove_outliers) { this->remove_outliers = remove_outliers;  }

    int getRebuildCount() { return this->rebuild_count_before_outlier_removal; }
    void setRebuildCount(int rc) { this->rebuild_count_before_outlier_removal = rc;  }


	void setDistanceMetric(std::string metric) { this->distanceFunc = assignDistanceMetric(metric); }
    double calculateDistance(Vec A, Vec B) { return this->distanceFunc(A, B); }

    void setClusterSizeMetric(std::string metric) { this->clusterSizeFunc = assignClusterSizeMetric(metric); }
//    double calculateClusterSize(std::shared_ptr<CF> cf) { return this->clusterSizeFunc(cf); }
	double calculateClusterSize(std::shared_ptr<CF> cf, Vec point) { return this->clusterSizeFunc(cf, point); }

    std::shared_ptr<CFTree>& getTree() { return this->tree; }
	void setTree(std::shared_ptr<CFTree> tree) { this->tree = tree; }
    void makeTree() {
	    this->tree.reset();
        this->tree = std::make_shared<CFTree>();
    }

//    std::shared_ptr<CFTree> getNewTree() { return this->new_tree; }
//    std::shared_ptr<CFTree> makeNewTree() {
//        this->new_tree.reset();
//        this->new_tree = std::make_shared<CFTree>();
//        return this->new_tree;
//    }

    void resetOutliers() {
	    this->outliers.clear();
	}

    /// for check_tree
    int lvl;
};


#endif