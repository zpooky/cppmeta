class TestBase { //
};

class /**/ TestBase2 /*:*/ /**/ /*TestBase*/ /**/ { //
                                                    //}
/**/}/**/;

class TestSub : public virtual TestBase {
  //};
};

class TestSub1 : virtual private TestBase {};

class TestSub2 : virtual private TestBase {
  /*};*/
};

class TestSub3 : private TestBase {
  //
};

class TestSub4 : virtual TestBase {
  //
};

class TestSub5 : TestBase {};

class TestSub6 : public virtual TestBase, virtual public TestBase2 {
  //};
};

class TestSub7 : virtual private TestBase, TestBase2 {};

class TestSub8 : virtual private TestBase, protected TestBase2 {
  /*};*/
};

class TestSub9 : private TestBase, TestBase2 {
  //
};

class TestSub10 : virtual TestBase, TestBase2 {
  //
};

class TestSub11 : TestBase, TestBase2 {};

class TestSub12 /*:*/ /*TestBase,*/ // TestBase2
{};

template <typename T>
class TestSub13 {};

template <typename>
class TestSub14 {};

template <typename T, typename B>
class TestSub15 {};

template <typename T = int>
class TestSub16 {};

template <typename T, typename B = int>
class TestSub17 {};
