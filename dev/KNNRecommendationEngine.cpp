/*
 * KNNRecommendationEngine.cpp
 *
 */

#include "KNNRecommendationEngine.h"
#include <map>
#include <iostream>
#include <vector>

using namespace std;

KNNRecommendationEngine::KNNRecommendationEngine() {
	objectFeatureMap = new map<int, map<int,int> >();
	nearestNeighbours = new map<int, map<int,float> >();
	recommendedFeaturesMap = new map<int,map<int, int> >();
	k = 3;
	noRecommendations = 3;
	period = 6*60*60;

	t = new thread(&KNNRecommendationEngine::computeRecommendationAsync, this);
	recomm_ds_mutex = new mutex();
	period_mutex = new mutex();
}

KNNRecommendationEngine::KNNRecommendationEngine(int period){
	objectFeatureMap = new map<int, map<int,int> >();
	nearestNeighbours = new map<int, map<int,float> >();
	recommendedFeaturesMap = new map<int,map<int, int> >();
	k = 3;
	noRecommendations = 3;
	this->period = period;

	t = new thread(&KNNRecommendationEngine::computeRecommendationAsync, this);
	recomm_ds_mutex = new mutex();
	period_mutex = new mutex();
}

KNNRecommendationEngine::~KNNRecommendationEngine() {

}

/*
 * private methods
 */

/*
 * Given two vectors compute the cosine similarity between the two vectors
 * (maps) and returns the similarity value
 */
float KNNRecommendationEngine::computeSimilarity(map<int, int> *vector1,
		map<int, int> *vector2) {
	map<int,int>::iterator vectorItr1;
	map<int,int>::iterator vectorItr2;
	float numerator = 0;
	float den1 = 0;
	float den2 = 0;

	vectorItr1 = vector1->begin();
	while(vectorItr1 != vector1->end()){
		vectorItr2 = vector2->find(vectorItr1->first);
		if(vectorItr2 != vector2->end()){
			numerator += vectorItr1->second * vectorItr2->second;
		}
		den1 += vectorItr1->second * vectorItr1->second;
		vectorItr1++;
	}

	vectorItr2 = vector2->begin();
	while(vectorItr2 != vector2->end()){
		den2 += vectorItr2->second * vectorItr2->second;
		vectorItr2++;
	}
	return (numerator/ (sqrt(den1) * sqrt(den2)) );
}

/*
 * Takes a map of a particular object (obj1) and adds a second object (obj2)
 * to this map if the similarity value passed is greater than all the existing
 * similarity values.
 */
bool KNNRecommendationEngine::addNearestNeighbour(map<int,float> *similarityMap,
		int objectId, float similarity) {
	map<int,float>::iterator simIterator;
	int size;
	float min = 0;
	int minObj;

	if(similarity == 1){
		// In case when the two users have visited the exact
		return false;
	}
	if(similarityMap->size() == this->k){
		simIterator = similarityMap->begin();
		if(simIterator != similarityMap->end()){
			min = simIterator->second;
			minObj = simIterator->first;
		}
		while(simIterator != similarityMap->end()){
			if(simIterator->second < min){
				min = simIterator->second;
				minObj = simIterator->first;
			}
			simIterator++;
		}
		if(similarity > min){
			simIterator = similarityMap->find(minObj);
			similarityMap->erase(simIterator);
			similarityMap->insert(pair<int,float>(objectId,similarity));
		}
	}else{
		similarityMap->insert(pair<int,float>(objectId,similarity));
	}

	return false;
}

/*
 * This method adds a recommended feature to the given map
 * if the frequency of this feature is greater than set of already
 * existing recommendations.
 */
bool KNNRecommendationEngine::addRecommendedFeature(map<int, int>* recommendMap,
		int featureId, int frequency) {
	map<int,int>::iterator featureMapItr;
	int size;
	int min = 0;
	int minFeature;

	if(recommendMap->size() == noRecommendations){
		featureMapItr = recommendMap->begin();
		if(featureMapItr != recommendMap->end()){
			min = featureMapItr->second;
			minFeature = featureMapItr->first;
		}
		featureMapItr++;
		while(featureMapItr != recommendMap->end()){
			if(featureMapItr->second < min){
				min = featureMapItr->second;
				minFeature = featureMapItr->first;
			}
			featureMapItr++;
		}
		if(frequency > min){
			featureMapItr = recommendMap->find(minFeature);
			recommendMap->erase(featureMapItr);
			recommendMap->insert(pair<int,int>(featureId,frequency));
			return true;
		}
	}else{
		recommendMap->insert(pair<int,int >(featureId,frequency));
	}
	return true;
}

/*
 * Iterates through the objects feature map to compute the set compute
 * the similarities between the objects. Updates the similarityMap object
 * to store k nearest neighbour IDs.
 */
bool KNNRecommendationEngine::computeKNN() {
	map<int,map<int,int> >::iterator iIterator;
	map<int,map<int,int> >::iterator jIterator;
	map<int,float> *similarityMap;
	float similarity;

	iIterator = objectFeatureMap->begin();
	while(iIterator != objectFeatureMap->end()){
		similarityMap = &(nearestNeighbours->find(iIterator->first)->second);
		jIterator = objectFeatureMap->begin();
		while(jIterator != objectFeatureMap->end()){
			if(jIterator != iIterator){
				similarity = computeSimilarity(&(iIterator->second),&(jIterator->second));
				addNearestNeighbour(similarityMap,jIterator->first,similarity);
			}
			jIterator++;
		}
		iIterator++;
	}
	return true;
}

/*
 * Uses the nearest neighbours map to lookup the closest features of
 * similar users to generate and populates the recommendations.
 */
bool KNNRecommendationEngine::generateRecommendations() {
	map<int,map<int,float> >::iterator nearestMapItr;
	map<int,float>::iterator neighbourMapItr;

	nearestMapItr = nearestNeighbours->begin();
	while(nearestMapItr != nearestNeighbours->end()){
		neighbourMapItr = (nearestMapItr->second).begin();
		while(neighbourMapItr != (nearestMapItr->second).end()){
			generateFeaturesToRecommend(nearestMapItr->first, neighbourMapItr->first);
			neighbourMapItr++;
		}
		nearestMapItr++;
	}
	return false;
}

/*
 * Store the recommended features vector for object1 from object2
 * Object1's recommendation vector at the end of the method
 * will have features in object2 that were not in object1
 */
bool KNNRecommendationEngine::generateFeaturesToRecommend(int objectId1,
		int objectId2) {
	map<int,map<int,int> >::iterator objFeatureItr;
	map<int,int> featuresMap1;
	map<int,int> featuresMap2;
	map<int,int>::iterator featuresMap1Itr;
	map<int,int>::iterator featuresMap2Itr;
	map<int, int> *featureFreqMap;

	objFeatureItr = objectFeatureMap->find(objectId1);
	if( objFeatureItr == objectFeatureMap->end()){
		return false;
	}
	featuresMap1 = objFeatureItr->second;

	objFeatureItr = objectFeatureMap->find(objectId2);
	if( objFeatureItr == objectFeatureMap->end()){
		return false;
	}
	featuresMap2 = objFeatureItr->second;

	featuresMap2Itr = featuresMap2.begin();
	while(featuresMap2Itr != featuresMap2.end()){
		featuresMap1Itr = featuresMap1.find(featuresMap2Itr->first);
		if(featuresMap1Itr == featuresMap1.end()){
			featureFreqMap = &(recommendedFeaturesMap->find(objectId1)->second);
			addRecommendedFeature(featureFreqMap,featuresMap2Itr->first,featuresMap2Itr->second);
		}
		featuresMap2Itr++;
	}
	return true;
}


/*
 * public methods
 */

/*
 * Checks for the presence of 'objectId' in the system.
 * If the objectId is not present, the object is added
 * and a feature entry is created against that object.
 * If the object already exists, a feature is created if
 * it does not exist else the frequency of the feature
 * is incremented.
 */
bool KNNRecommendationEngine::addObjectFeature(int objectId, int featureId) {
	map<int,map<int,int> >::iterator objectFeatureItr;
	map<int,int> *featureMap;
	map<int,int>::iterator featureItr;
	map<int,float>* similarityMap;

	objectFeatureItr = objectFeatureMap->find(objectId);
	if (objectFeatureItr != objectFeatureMap->end()){
		featureMap = &objectFeatureItr->second;
		featureItr = featureMap->find(featureId);
		if(featureItr != featureMap->end()){
			++featureItr->second;
		}else{
			featureMap->insert(pair<int, int>(featureId, 1));
		}
		return true;
	}else{
		map<int, int> *featureFreqMap = new map<int, int>();
		featureMap = new map<int,int>();
		featureMap->insert(pair<int,int>(featureId,1));

		recommendedFeaturesMap->insert(pair<int,map<int, int> >(objectId,*featureFreqMap));
		objectFeatureMap->insert(pair<int,map<int,int> >(objectId,*featureMap));

		similarityMap = new map<int,float>();
		nearestNeighbours->insert(pair<int,map<int,float> >(objectId,*similarityMap));
		return true;
	}
	return false;
}


/*
 * Sets the value of k, which the number of nearest neighbors to consider while
 * generating recommendations.
 * Takes effect during the next run of the recommendation engine computation.
 */
void KNNRecommendationEngine::setK(int k) {
	recomm_ds_mutex->lock();
	this->k = k;
	recomm_ds_mutex->unlock();
}

/*
 * Used to tune the number of recommendations generated by the recommendation engine.
 * Takes effect during the next run of the recommendation engine computation.
 */
void KNNRecommendationEngine::setNoRecommendations(int n) {
	recomm_ds_mutex->lock();
	this->noRecommendations = n;
	recomm_ds_mutex->unlock();
}


/*
 * Sets the periodicity with which the recommendation engine has to be
 * run to calculate recommendations.
 * The new periodicity takes into effect only during the next run of the
 * recommendation engine.
 */
void KNNRecommendationEngine::setPeriod(int period) {
	period_mutex->lock();
	this->period = period;
	period_mutex->unlock();
}

/*
 * returns a map of the recommended object IDs and their frequencies,
 * which is a measure of their likelihood of recommendations.
 */
map<int,int>* KNNRecommendationEngine::getRecommendations(int objectId){
	map<int, map<int,int> >::iterator recommendationItr;

	recommendationItr = recommendedFeaturesMap->find(objectId);
	if(recommendationItr != recommendedFeaturesMap->end()){
		return &recommendationItr->second;
	}
	else{
		return NULL;
	}
}


/*
 * A separate thread is created for the execution of this method.
 * The thread computes the recommendation once every 6 hours.
 * The periodicity of execution can be modified by calling the
 * setPeriod(int) method.
 * This method is a public method, is not exposed to the end user.
 * It is public because the thread object needs access.
 */
void KNNRecommendationEngine::computeRecommendationAsync() {
	while(true){
		period_mutex->lock();
		sleep(this->period);
		period_mutex->unlock();

		recomm_ds_mutex->lock();
		computeKNN();
		generateRecommendations();
		//printRecommendedFeatures();
		recomm_ds_mutex->unlock();
	}
}

/*
 * Does a synchronous computation of the recommendationst for objects
 */
void KNNRecommendationEngine::computeRecommendationSync(){
	recomm_ds_mutex->lock();
	computeKNN();
	generateRecommendations();
	recomm_ds_mutex->unlock();
}
