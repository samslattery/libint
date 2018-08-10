#include "catch.hpp"

#include <type_traits>

#include <libint2/config.h>
#include "../../src/bin/test_eri/eri.h"

#ifdef LIBINT_HAS_MPFR
TEST_CASE("Boys reference values", "[boys]") {
  using scalar_type = libint2::scalar_type;

  const int mmax = 40;
  const int nbits_ref = 1024;
  std::vector<LIBINT2_REF_REALTYPE> T_ref_values;
  std::vector<double> T_values;
  T_ref_values.push_back(LIBINT2_REF_REALTYPE(0, nbits_ref));
  T_values.push_back(0);
  for(int i=0; i!=12; ++i) {
    using std::sqrt;
    using std::pow;
    T_ref_values.push_back(pow(sqrt(LIBINT2_REF_REALTYPE(10, nbits_ref)), i) / 1000);
    T_values.push_back(pow(sqrt(10.),i) / 1000.);
  }

  if (!std::is_same<LIBINT2_REF_REALTYPE,scalar_type>::value) {
    auto Fm_ref_values = new LIBINT2_REF_REALTYPE[mmax+1];
    auto Fm_values = new scalar_type[mmax+1];
    for(int t=0; t!=T_ref_values.size(); ++t) {
      auto Tref = T_ref_values[t];
      auto T = T_values[t];
//      std::cout << "std::numeric_limits<LIBINT2_REF_REALTYPE>::epsilon() = " << libint2::get_epsilon(Tref).get_d() << std::endl;
      libint2::FmEval_Reference<LIBINT2_REF_REALTYPE>::eval(Fm_ref_values, Tref, mmax);
      auto fm_eval = libint2::FmEval_Chebyshev7<scalar_type>::instance(mmax);
      //auto fm_eval = libint2::FmEval_Taylor<scalar_type, 3>::instance(mmax);
      fm_eval->eval(Fm_values, T, mmax);
      for(int m=0; m<=mmax; ++m) {
        const LIBINT2_REF_REALTYPE value = libint2::sstream_convert<LIBINT2_REF_REALTYPE>(Fm_values[m]);
        const LIBINT2_REF_REALTYPE abs_error = abs(Fm_ref_values[m] - value);
        const LIBINT2_REF_REALTYPE relabs_error = abs(abs_error / Fm_ref_values[m]);
        printf("m=%d T=%e ref_value=%20.15e abs_error=%e, relabs_error=%e\n", m, T, Fm_ref_values[m].get_d(), abs_error.get_d(), relabs_error.get_d());
        REQUIRE(abs_error.get_d() == Approx(0.0).margin(10*std::numeric_limits<scalar_type>::epsilon()) );  // Boys engine
        REQUIRE(relabs_error.get_d() == Approx(0.0).margin(1000*std::numeric_limits<scalar_type>::epsilon()) );
      }
    }
  }
}
#endif  // LIBINT_HAS_MPFR