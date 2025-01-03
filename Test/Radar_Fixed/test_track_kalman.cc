#include "track_kalman.hh"

#include <iostream>

using namespace std;
int test_predict()
{

    KalmanPredictor predictor = KalmanPredictor(1);
    GaussianState prior_state = GaussianState(Vector4r(0, 1, 0, 1), Matrix44r::Identity(), 0);
    GaussianState predict_state;


    predictor.predict(predict_state, prior_state, 1);

    cout << predict_state.state_vector << endl;
    cout << predict_state.covar << endl;
    return 0;
}

int main()
{
    test_predict();
    return 0;
}
