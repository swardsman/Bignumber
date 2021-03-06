#include "bignumber.h"
#include <regex>
#include <exception>
#include <cstdio>


/******************************************
 * 对字符串进行检查其是否能够正确表示一个数值，能正确表示
 * 一个数值的字符串应满足：
 * 1.当其开头为‘+’或‘-’时，‘+’可以省略，‘-’不可省略，且符号位只有一位
 * 2.数值字符串中只能存在0-9、‘-’、‘+’、‘.’四种字符,且出数字之外的字符
 * 最多只能出现一次
 * 3.数字前面可以出现连续的0，比如000123
 * 4.整数结尾可以带有‘.‘，如123.
 * 5.小数部分之后可以存在连续的0，比如1.2340000
 * 6.当字符串中存在小数点时，小数点前面必须存在一个或多个数字，
 * 如.123、+.123等视为非法数值字符串
 **********************************************/
static bool checkString(const std::string& value){
    std::regex pattern("([-+]{0,1})([0-9]*)([0-9]\\.{0,1}){0,1}([0-9]*)");
    return std::regex_match(value,pattern);
}

void formatString(std::string & numStr){
    std::string tmp;
    size_t nonZeroBegin = 0;
    if (numStr[0] == '-' || numStr[0] == '+') {
        tmp.push_back(numStr[0]);
        nonZeroBegin = numStr.find_first_not_of('0',1);
    }
    else{
        tmp.push_back('+');
        nonZeroBegin = numStr.find_first_not_of('0');
    }
    if (nonZeroBegin == std::string::npos){
        numStr = "+0";
        return;
    }
    size_t endPos = 0,dotPos = 0;
    if ((dotPos = numStr.find('.')) != std::string::npos){
        size_t nonZeroEnd   = numStr.find_last_not_of('0');
        if (dotPos < nonZeroEnd) endPos = nonZeroEnd;
        else endPos = dotPos - 1;
    }
    else endPos = numStr.size() - 1;
    if (nonZeroBegin == dotPos) nonZeroBegin -= 1;
    do {
        tmp.push_back(numStr[nonZeroBegin]);
    } while ((++nonZeroBegin) <= endPos);
    numStr = tmp;
}

Bignumber::Bignumber() : value_(new BignumberImpl(""))
{
}

Bignumber::Bignumber(const std::string & value){
    if (!checkString(value)) throw std::invalid_argument("Input value is not a valid number!");
    std::string v = value;
    formatString(v);
    value_ = boost::shared_ptr<BignumberImpl>(new BignumberImpl(v));
}

Bignumber::Bignumber(const Bignumber &other){
	if (value_ == other.value_) return;
	value_ = other.value_;
}
Bignumber::Bignumber(Bignumber && other){
	value_ = other.value_;
}
Bignumber::Bignumber(const long long & num){
    constexpr int longMax = 24;//long long类型所能表示的最大数字的长度为20
    char v[longMax];
    std::memset(v,0,longMax);
    std::sprintf(v,"%lld",num);
    std::string value(v);
    formatString(value);
    value_ = boost::shared_ptr<BignumberImpl> (new BignumberImpl(value));
}
/*Bignumber::Bignumber(double num){
    constexpr int doubleMax = 320;//double类型所能表示的最大数字长度为308：1.79769e+308、2.22507e-308
    char v[doubleMax];
    std::memset(v,0,doubleMax);
    std::sprintf(v,"%g",num);
    std::cout<<v<<std::endl;
    std::string value(v);
    formatString(value);
    value_ = boost::shared_ptr<BignumberImpl> (new BignumberImpl(value));
}
TEST(Constructor,BignumberDoubleConstructor){
    EXPECT_EQ(Bignumber(1.234),Bignumber("1.234"));
    EXPECT_EQ(Bignumber(-1.234),Bignumber("-1.234"));
    EXPECT_EQ(Bignumber(-1.23400),Bignumber("-1.234"));
    EXPECT_EQ(Bignumber(0.23456),Bignumber("0.23456"));

#include <limits>
    std::cout<<std::numeric_limits<double>::max()<<std::endl;
    std::cout<<std::numeric_limits<double>::min()<<std::endl;
    EXPECT_EQ(Bignumber(std::numeric_limits<double>::max()),Bignumber("179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368"));
//    EXPECT_EQ(Bignumber(std::numeric_limits<double>::min()),Bignumber("0"));
}
*/
Bignumber& Bignumber::operator = (const Bignumber& other){
	if (value_ != other.value_) 
		value_  =   other.value_;
    return *this;
}

Bignumber& Bignumber::operator = (Bignumber&& other){
    value_  =   other.value_;
    return *this;
}

Bignumber::~Bignumber(){}

/*************************************
 * Bignumber的加减和乘法，内部直接调用
 * BignumberImpl的加减乘法
 * ***********************************/
Bignumber operator + (const Bignumber& num1,const Bignumber& num2){
    return Bignumber(*(num1.value_) + *(num2.value_));
}
Bignumber& Bignumber::operator += (const Bignumber & other){
    *this = *this + other;
    return *this;
}
Bignumber operator - (const Bignumber& num1,const Bignumber& num2){
    return Bignumber(*(num1.value_) - *(num2.value_));
}
Bignumber& Bignumber::operator -= (const Bignumber & other){
    *this = *this - other;
    return *this;
}
Bignumber operator * (const Bignumber& num1,const Bignumber& num2) {
	return Bignumber(*(num1.value_) * (*(num2.value_)));
}
Bignumber& Bignumber::operator *= (const Bignumber & other) {
	*this = *this * other;
	return *this;
}

/***************************************
 * Bignumber的比较操作符
 * *************************************/
bool operator == (const Bignumber & num1,const Bignumber & num2) {return *(num1.value_) == *(num2.value_);}
bool operator != (const Bignumber & num1,const Bignumber & num2){return *(num1.value_) != *(num2.value_);};
bool operator < (const Bignumber & num1,const Bignumber & num2){return *(num1.value_) < *(num2.value_);};
bool operator > (const Bignumber & num1,const Bignumber & num2){return *(num1.value_) > *(num2.value_);};
bool operator <= (const Bignumber & num1,const Bignumber & num2){return *(num1.value_) <= *(num2.value_);};
bool operator >= (const Bignumber & num1,const Bignumber & num2){return *(num1.value_) >= *(num2.value_);};

/****************************************
 * Bignumber的输入、输出操作符
 * **************************************/
std::istream& operator >> (std::istream& in,Bignumber& num){
    std::string v;
    in>>v;
    if (!checkString(v)) throw  std::invalid_argument("Input value is not a valid number!");
    formatString(v);
    num.value_ = boost::shared_ptr<BignumberImpl> (new BignumberImpl(v));
    return in;
}
std::ostream& operator << (std::ostream& out,const Bignumber& num){
    out<<(num.value_->getValue());
    return out;
}
