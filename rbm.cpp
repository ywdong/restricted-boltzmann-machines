/*
 * =====================================================================================
 *
 *       Filename:  rbm.cpp
 *
 *    Description:  rbm of cpp implementation
 *
 *        Version:  1.0
 *        Created:  2012年12月18日 22时34分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Weidong Yang (cn), ywdong@mail2.sysu.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */
#include <cassert>
#include <cstdlib>
#include <iomanip>   
#include <iostream>  
#include <ctime>
#include <cmath>
#include "matrix.h"

using namespace std;

class RBM {

    private:
        int m_num_visible;
        int m_num_hidden;
        double m_learning_rate;
        Matrix m_weights;

    public:
        RBM(int num_visible, int num_hidden, double learning_rate)
        {
            m_num_visible = num_visible;
            m_num_hidden = num_hidden;
            m_learning_rate = learning_rate;

            Matrix tmp(num_visible+1, num_hidden+1);

            for (int i=0; i<num_hidden+1; i++)
            {
                tmp.m_values[0][i] = 0;
            }
            for (int j=0; j<num_visible+1; j++)
            {
                tmp.m_values[j][0] = 0;
            }
            srand(time(NULL));
            // init values in weights
            cout << "Print weights:" << endl;
            for (int i=1; i<num_visible+1; i++)
            {
                for (int j=1; j<num_hidden+1; j++)
                {
                    tmp.m_values[i][j] = rand() / (RAND_MAX+1.0) * 0.1;
                }
            }

            m_weights = tmp;
            cout << "m_weights matrix :" << endl;
            m_weights.Print(20);

        }
        ~RBM()
        {
        }
        void Train(const Matrix& data, int max_epochs = 1000);
        double* Run_visible(Matrix data);
        double* Run_hidden(Matrix data);
};

void RBM::Train(const Matrix& data, int max_epochs)
{
    assert(data.m_col == m_num_visible+1);
    double error = 0.0;

    Matrix pos_hidden_activations;
    Matrix pos_hidden_probs;
    Matrix pos_hidden_states;
    Matrix neg_visible_activations;
    Matrix neg_visible_probs;
    Matrix neg_hidden_activations;
    Matrix neg_hidden_probs;
    Matrix neg_associations;
    Matrix pos_associations;

    for (int epochs=0; epochs<max_epochs; epochs++)
    {

        //Clamp to the data and sample from the hidden units
        //(This is the "positive CD phase", aka the reality phase.)
        pos_hidden_activations = data * m_weights;
        pos_hidden_probs = pos_hidden_activations.Logistic();
        pos_hidden_states = pos_hidden_probs.Random();
        pos_associations = data.Transpose() * pos_hidden_probs;        

        // Reconstruct the visible units and sample again fromthe hidden units.
        // (This is the "negative CD phase", aka the daydreaming phase.)
        neg_visible_activations = pos_hidden_states * m_weights.Transpose();
        neg_visible_probs = neg_visible_activations.Logistic();        

        // Fix the bias unit
        for (int i=0; i<data.m_row; i++)
            neg_visible_probs.m_values[i][0] = 1;

        neg_hidden_activations = neg_visible_probs * m_weights;
        neg_hidden_probs = neg_hidden_activations.Logistic();

        // We're using the activation *probabilities* when computing associations, 
        // not the states themselves.
        neg_associations = neg_visible_probs.Transpose() * neg_hidden_probs;

        // Update weights.
        for (int i=0; i<m_weights.m_row; i++)
        {
            for (int j=0; j<m_weights.m_col; j++)
            {
                m_weights.m_values[i][j] += m_learning_rate * 
                    ((pos_associations.m_values[i][j] - neg_associations.m_values[i][j]) / data.m_row);
            }
        }

        cout << "weights:" << endl;
        m_weights.Print(20);

        for (int i=0; i<data.m_row; i++)
        {
            for (int j=0; j<data.m_col; j++)
            {
                error += pow((data.m_values[i][j] - neg_visible_probs.m_values[i][j]), 2);
            }
        }
        std::cout << "Epoch " << epochs << " : error is " <<  error << std::endl;
        error = 0.0;
    }
}

int main()
{
    //num_vis, num_hid, learing_rate
    RBM rbm(6, 2, 0.1);
    double array[6][7] = {
        {1,1,1,1,0,0,0},
        {1,1,0,1,0,0,0},
        {1,1,1,1,0,0,0},
        {1,0,0,1,1,1,0},
        {1,0,0,1,1,0,0},
        {1,0,0,1,1,1,0},
    };
    Matrix train(6, 7, (double**)array);
    rbm.Train(train, 1000);
    return 0;
}
