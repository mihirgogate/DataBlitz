/*
 * KNNRecommendationEngine.h
 *
 */

#ifndef KNNRECOMMENDERSTORE_H_
#define KNNRECOMMENDERSTORE_H_

using namespace std;

#include <map>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <math.h>


class KNNRecommendationEngine {
private:
	map<int, map<int, int> > *objectFeatureMap;
	map<int, map<int, float> > *nearestNeighbours;
	map<int, map<int, int> > *recommendedFeaturesMap;
	int k;
	int noRecommendations;
	unsigned int period;
	thread *t;
	mutex* recomm_ds_mutex;
	mutex* period_mutex;

	float computeSimilarity(map<int,int> *vector1,map<int,int> *vector2);
	bool addNearestNeighbour(map<int,float> *similarityMap,int objectId2,float similarity);
	bool addRecommendedFeature(map<int,int> *recommendMap,int featureId,int frequency);
	bool computeKNN();
	bool generateRecommendations();
	bool generateFeaturesToRecommend(int objectId1,int objectId2);

public:
	KNNRecommendationEngine();
	KNNRecommendationEngine(int period);
	virtual ~KNNRecommendationEngine();

	bool addObjectFeature(int objectId,int featureId);
	void setK(int k); //SETNEIGHBOURS [k]
	void setNoRecommendations(int n); //SETNOOFRECOMMENDATIONS [noOfRecommendations]
	void setPeriod(int period); //SETPERIOD [period]
	map<int,int>* getRecommendations(int objectId); //GETRECOMMENDATION [objectId]

	void computeRecommendationAsync();
	void computeRecommendationSync(); //COMPUTEREC

};

#endif /* KNNRECOMMENDERSTORE_H_ */

