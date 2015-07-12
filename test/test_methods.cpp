#include <libunittest/all.hpp>
#include <spgl1++/all.hpp>
#include <armadillo>

#ifndef DATADIR
#define DATADIR "."
#endif

using namespace unittest::assertions;

//namespace libspgl1 {
//namespace vector {
//	template<>
//    arma::vec abs(const arma::vec& a){
//    	return arma::abs(a);
//    }
//
//} // vector
//} // libspgl1

namespace libspgl1 {
namespace matrix{
	template<>
	arma::vec matvec(const arma::vec& A, const arma::vec& b){
		return A*b;
	}
}
}

struct test_minimal : unittest::testcase<> {

	/*
	 * The csv files in the examples below were generated using the MATLAB code
	 * provided on the spgl1 website: https://www.math.ucdavis.edu/~mpf/spgl1/examples/spgexamples.html
	 *
	 * rand('twister',0); randn('state',0);
	 * m = 50; n = 128; k = 14;                   % No. of rows (m), columns (n), and nonzeros (k)
	 * [A,Rtmp] = qr(randn(n,m),0);               % Random encoding matrix with orthogonal rows
	 * A  = A';                                   % ... A is m-by-n
	 * p  = randperm(n); p = p(1:k);              % Location of k nonzeros in x
	 * x0 = zeros(n,1); x0(p) = randn(k,1);       % The k-sparse solution
	 * b  = A*x0;                                 % The right-hand side corresponding to x0
	*/

    static void run()
    {
        UNITTEST_CLASS(test_minimal)
        UNITTEST_RUN(test_dot_product)
        UNITTEST_RUN(test_norm_l1)
        UNITTEST_RUN(test_norm_l2)
        UNITTEST_RUN(test_NormL1_primal_with_weighting_equal_to_one)
        UNITTEST_RUN(test_NormL1_primal_with_weights_less_than_one)
        UNITTEST_RUN(test_execute_spgl1_method)
        UNITTEST_RUN(test_projectI)
        UNITTEST_RUN(test_initialization)
        UNITTEST_RUN(test_abs)
        UNITTEST_RUN(test_sign)
        UNITTEST_RUN(test_get_element)
        UNITTEST_RUN(test_set_element)
        UNITTEST_RUN(test_elementwise_multiplication)
        UNITTEST_RUN(test_max)
        //UNITTEST_RUN(test_NormL1_primal_weights_cannot_be_negative)
        //UNITTEST_RUN(test_NormL1_primal_weights_should_be_less_than_or_equal_to_one)
    }

    void test_dot_product()
    {
    	arma::mat row_vector1(2,1);
    	arma::mat row_vector2(2,1);
    	row_vector1.fill(1.0);
    	row_vector2 = row_vector1;
    	assert_equal(2.0, libspgl1::vector::dot<double>(row_vector1, row_vector2));
    }

    void test_abs()
    {
    	arma::vec vector1{1.0,-1.0};
    	arma::vec result = libspgl1::vector::abs<arma::vec>(vector1);
    	arma::vec expected{1.0,1.0};
    	for (size_t i=0;i<libspgl1::vector::n_elem(vector1);i++){
    		assert_equal(libspgl1::vector::get_element<double>(expected, i),
    				     libspgl1::vector::get_element<double>(result, i));
    	}
    }

    void test_sign()
    {
    	arma::vec vector1{2.0,-2.0};
    	arma::vec result = libspgl1::vector::sign<arma::vec>(vector1);
    	arma::vec expected{1.0,-1.0};
    	for (size_t i=0;i<libspgl1::vector::n_elem(vector1);i++){
    		assert_equal(libspgl1::vector::get_element<double>(expected, i),
    				     libspgl1::vector::get_element<double>(result, i));
    	}
    }

    void test_elementwise_multiplication()
    {
    	arma::vec vector1{2.0,-2.0};
    	arma::vec vector2{2.0,3.0};
    	arma::vec result = libspgl1::vector::elementwise_multiplication<arma::vec>(vector1, vector2);
    	arma::vec expected{4.0,-6.0};
    	for (size_t i=0;i<libspgl1::vector::n_elem(vector1);i++){
    		assert_equal(libspgl1::vector::get_element<double>(expected, i),
    				     libspgl1::vector::get_element<double>(result, i));
    	}
    }

    void test_get_element()
    {
    	arma::vec vector1{1.0};
    	assert_equal(vector1(0), libspgl1::vector::get_element<double>(vector1,0));
    }

    void test_set_element()
    {
    	arma::vec vector1{1.0, 5.0};
    	size_t element_index = 1;
    	double expected_new_element = 10.0;
    	libspgl1::vector::set_element(vector1, element_index, expected_new_element);
    	assert_equal(expected_new_element, libspgl1::vector::get_element<double>(vector1, element_index));
    }

    void test_norm_l1()
    {
    	arma::vec a = {-1,1};
    	assert_equal(2.0, libspgl1::math::norm<double>(a, 1));
    }

    void test_max()
    {
    	arma::vec a1 = {-1,1};
    	arma::vec a2 = {-100, 0, 50};
    	assert_equal(1.0, libspgl1::math::max<double>(a1));
    	assert_equal(50.0, libspgl1::math::max<double>(a2));
    }

    void test_norm_l2()
    {
    	arma::vec a = {3,4};
    	assert_equal(5.0, libspgl1::math::norm<double>(a, 2));
    }

    void test_NormL1_primal_with_weighting_equal_to_one()
    {
    	arma::vec v = arma::linspace<arma::vec>(-1, 1, 2);
    	arma::vec weights = arma::ones<arma::vec>(2);
    	assert_equal(2.0, libspgl1::math::NormL1_primal<double>(v, weights));
    }

    void test_NormL1_primal_with_weights_less_than_one()
    {
    	arma::vec v = arma::linspace<arma::vec>(-2, 2, 2);
    	arma::vec weights = arma::colvec(2);
    	weights.fill(0.5);
    	assert_equal(2.0, libspgl1::math::NormL1_primal<double>(v, weights));
    }

    void test_NormL1_primal_weights_cannot_be_negative()
    {
    	arma::vec v = arma::linspace<arma::vec>(-2, 2, 2);
    	arma::vec weights = arma::colvec(2);
    	weights.fill(-0.5);
    	assert_throw<std::invalid_argument>([&v, &weights](){ libspgl1::math::NormL1_primal<double>(v, weights); }, SPOT);
    }

    void test_NormL1_primal_weights_should_be_less_than_or_equal_to_one()
    {
    	arma::vec v = arma::linspace<arma::vec>(-2, 2, 2);
    	arma::vec weights = arma::colvec(2);
    	weights.fill(1.5);
    	assert_throw<std::invalid_argument>([&v, &weights](){ libspgl1::math::NormL1_primal<double>(v, weights); }, SPOT);
    }

    void test_execute_spgl1_method(){
    	arma::mat A;
    	A.load(std::string(DATADIR) + "/A_basic.csv");
    	arma::mat At = A.t();
    	arma::vec x;
    	x.load(std::string(DATADIR) + "/x0_basic.csv");
    	arma::vec b = A*x;
    	arma::vec x0 = At*b;
    	arma::vec x_soln = libspgl1::spgl1(A, At, b, x0);

    	/*
    	f: 3.66216503085451
    	gstep: 100000
    	gNorm: 1.00110911978155
    	rNorm: 2.70634995181869
    	gap: 0
    	rGap: 0
    	aError1: 2.70634995181869
    	aError2: 3.66216503085451
    	rError1: 1
    	rError2: 1
    	testRelChange1: 1
    	testRelChange2: 1
    	testUpdateTau: 1
    	tauOld: 0
        parameters.tau: 7.31621550236924 */



    }

    void test_projectI(){
    	arma::vec x_before_project;
    	arma::vec x_after_project_expected;
    	x_before_project.load(std::string(DATADIR) + "/x_to_project.csv");
    	x_after_project_expected.load(std::string(DATADIR) + "/x_after_projection.csv");
    	arma::vec x_after_project = libspgl1::projectI(x_before_project, 100.0);
    	double norm_actual   = libspgl1::math::norm<double>(x_after_project, 1.0);
    	double norm_expected = libspgl1::math::norm<double>(x_after_project_expected, 1.0);
    	assert_approx_equal(norm_expected, norm_actual, 0.00001, SPOT);
    	//assert_approx_equal_containers(x_after_project_expected, x_after_project, 0.00001, SPOT);
    }

    void test_initialization(){
    	arma::mat A;
    	A.load(std::string(DATADIR) + "/A_basic.csv");
    	arma::mat At = A.t();
    	arma::vec x;
    	x.load(std::string(DATADIR) + "/x0_basic.csv");
    	arma::vec b = A*x;
    	arma::vec r = libspgl1::initialization::compute_r(A, b, x);
    	double f = libspgl1::initialization::compute_f(r);
    	assert_equal(0.0, f);
    	arma::vec g = libspgl1::initialization::compute_g(At, r);
    	for (auto element : g)
    		assert_equal(0.0, element);
    }

};
REGISTER(test_minimal);
