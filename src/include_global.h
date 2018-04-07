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

    int dimensions;
    int BF_B;
    int BF_L;
    double threshold_T;

	distanceFPtr distanceFunc;
    clusterSizeFPtr clusterSizeFunc;

	std::shared_ptr<CFTree> tree;

public:
	static Global& get()
	{
		static Global instance;
		return instance;
	}

	int getDimensions() { return this->dimensions; }
	void setDimensions(int dims) { this->dimensions = dims;  }

    int getBF_B() { return this->BF_B; }
    void setBF_B(int BF_B) { this->BF_B = BF_B;  }

    int getBF_L() { return this->BF_L; }
    void setBF_L(int BF_L) { this->BF_L = BF_L;  }

    double getThreshold_T() { return this->threshold_T; }
    void setThreshold_T(double t) { this->threshold_T = t;  }

    void setDistanceMetric(std::string metric) { this->distanceFunc = assignDistanceMetric(metric); }
    double calculateDistance(Vec A, Vec B) { return this->distanceFunc(A, B); }

    void setClusterSizeMetric(std::string metric) { this->clusterSizeFunc = assignClusterSizeMetric(metric); }
//    double calculateClusterSize(std::shared_ptr<CF> cf) { return this->clusterSizeFunc(cf); }
	double calculateClusterSize(std::shared_ptr<CF> cf, Vec point) { return this->clusterSizeFunc(cf, point); }

    std::shared_ptr<CFTree> getTree() { return this->tree; }
    std::shared_ptr<CFTree> makeNewTree() {
	    this->tree.reset();
        this->tree = std::make_shared<CFTree>();
        return this->tree;
    }

    /// for check_tree
    int lvl;
};


#endif