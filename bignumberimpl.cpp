#include "bignumberimpl.h"
#include <regex>

std::string BignumberImpl::getValue() const {
    std::string result(this->size(),'0');
    if (isNegetive()) result[0] = '-';
    else result[0] = '+';
    size_t i = 1;
    for (;i <= getIntegerSize();++i){
        result[i] = numStr_[i - 1];
    }
    if (getPrecision() != 0) {
        result[i] = '.';
        for (;i <= size() - 2;++i) result[i + 1] = numStr_[i - 1];
    }

    return result;
}

/******************************************
 * 两个代表整数的数值字符串相加，
 * NOTE：这两个字符串均应为格式化后的字符串，在本程序
 * 中用于两个BignumberImpl相加的内部实现
 * 返回的字符串为格式化后的字符串
 * ****************************************/
static std::string positiveAdd(const std::string & num1,size_t dot1,const std::string & num2,size_t dot2){
    //分别对两个数值字符串的整数和小数部分进行补零
    //整数部分补零：
    int intSizeNum1 = dot1 == 0 ? num1.size() - 1 : num1.size() - dot1 - 2;
    int intSizeNum2 = dot2 == 0 ? num2.size() - 1 : num2.size() - dot2 - 2;
    std::string integerNum1(std::max(intSizeNum1,intSizeNum2),'0');
    std::string integerNum2(std::max(intSizeNum1,intSizeNum2),'0');

    for (int i = integerNum1.size() - 1;i >= 0 && intSizeNum1 > 0;--i,--intSizeNum1)
        integerNum1[i] = num1[intSizeNum1];
    for (int i = integerNum2.size() - 1;i >= 0 && intSizeNum2 > 0;--i,--intSizeNum2)
        integerNum2[i] = num2[intSizeNum2];

    //小数部分补零
    std::string decimalNum1,decimalNum2;
    if (dot1 != 0 || dot2 != 0){
        decimalNum1 = std::string(std::max(dot1,dot2),'0');
        decimalNum2 = std::string(std::max(dot1,dot2),'0');
        for (size_t i = 0;i < decimalNum1.size() && num1.size() - dot1 + i < num1.size();++i)
            decimalNum1[i] = num1[num1.size() - dot1 + i];
        for (size_t i = 0;i < decimalNum2.size() && num2.size() - dot2 + i < num2.size();++i)
            decimalNum2[i] = num2[num2.size() - dot2 + i];
    }

    //开始进行计算
    int flag = 0;//进位标识
    //首先计算小数部分
    for (int i = decimalNum1.size() - 1;i >= 0;--i){
        int tmp = decimalNum1[i] - '0' + decimalNum2[i] - '0' + flag;
        if (tmp >= 10) flag = 1;else flag = 0;
        decimalNum1[i] = tmp % 10 + '0';
    }
    //计算整数部分
    for (int i = integerNum1.size() - 1;i >= 0;--i){
        int tmp = integerNum1[i] - '0' + integerNum2[i] - '0' + flag;
        if (tmp >= 10) flag = 1;else flag = 0;
        integerNum1[i] = tmp % 10 + '0';
    }

    std::string result;
    if (flag == 1) {
        if (decimalNum1.size() == 0)
            result = "1" + integerNum1;
        else result = "1" + integerNum1 + "." + decimalNum1;
    }
    else {
        if (decimalNum1.size() == 0)
            result = integerNum1;
        else result = integerNum1 + "." + decimalNum1;
    }

    //格式化结果并返回
    formatString(result);
    return result;
}



/******************************************
 * 两个代表整数的数值字符串相减，
 * NOTE：这两个字符串均应为格式化后的字符串，且必须保证num1>=num2,
 * 此函数在本程序中用于两个BignumberImpl相减的内部实现
 * 返回的字符串为格式化后的字符串
 * ****************************************/
static std::string positiveSub(const std::string & num1,size_t dot1,const std::string & num2,size_t dot2){
    //分别对两个数值字符串的整数和小数部分进行补零
    //整数部分补零：
    int intSizeNum1 = dot1 == 0 ? num1.size() - 1 : num1.size() - dot1 - 2;
    int intSizeNum2 = dot2 == 0 ? num2.size() - 1 : num2.size() - dot2 - 2;
    std::string integerNum1(std::max(intSizeNum1,intSizeNum2),'0');
    std::string integerNum2(std::max(intSizeNum1,intSizeNum2),'0');

    for (int i = integerNum1.size() - 1;i >= 0 && intSizeNum1 > 0;--i,--intSizeNum1)
        integerNum1[i] = num1[intSizeNum1];
    for (int i = integerNum2.size() - 1;i >= 0 && intSizeNum2 > 0;--i,--intSizeNum2)
        integerNum2[i] = num2[intSizeNum2];

    //小数部分补零
    std::string decimalNum1,decimalNum2;
    if (dot1 != 0 || dot2 != 0){
        decimalNum1 = std::string(std::max(dot1,dot2),'0');
        decimalNum2 = std::string(std::max(dot1,dot2),'0');
        for (size_t i = 0;i < decimalNum1.size() && num1.size() - dot1 + i < num1.size();++i)
            decimalNum1[i] = num1[num1.size() - dot1 + i];
        for (size_t i = 0;i < decimalNum2.size() && num2.size() - dot2 + i < num2.size();++i)
            decimalNum2[i] = num2[num2.size() - dot2 + i];
    }

    //开始进行计算
    int flag = 0;//进位标识
    //首先计算小数部分
    for (int i = decimalNum1.size() - 1;i >= 0;--i){
        int tmp = 0;
        if (decimalNum1[i] < decimalNum2[i]) {
            tmp = 10 + (decimalNum1[i] - '0') - (decimalNum2[i] - '0') - flag;
            flag = 1;
        }
        else {
            tmp = decimalNum1[i] - '0' - (decimalNum2[i] - '0') - flag;
            flag = 0;
        }
        decimalNum1[i] = tmp % 10 + '0';
    }
    //计算整数部分
    for (int i = integerNum1.size() - 1;i >= 0;--i){
        int tmp = 0;
        if (integerNum1[i] - flag < integerNum2[i]) {
            tmp = 10 + (integerNum1[i] - '0') - (integerNum2[i] - '0') - flag;
            flag = 1;
        }
        else {
            tmp = integerNum1[i] - '0' - (integerNum2[i] - '0') - flag;
            flag = 0;
        }
        integerNum1[i] = tmp % 10 + '0';
    }

    std::string result;
    if (decimalNum1.size() == 0) result = integerNum1;
    else result = integerNum1 + "." + decimalNum1;

    //格式化结果并返回
    formatString(result);
    return result;
}
BignumberImpl BignumberImpl::operator + (const BignumberImpl & other) {
    std::string result;
    if (this->isPositive()) {
        //两个正数相加
        if (other.isPositive())
            result = positiveAdd(this->getValue(),this->getPrecision(),\
                                                   other.getValue(),other.getPrecision());
        else {//正数加负数
            BignumberImpl tmp = other.getNegete();
            if (*this >= tmp)
                result = positiveSub(this->getValue(),this->getPrecision(),\
                                                tmp.getValue(),tmp.getPrecision());
            else {
                result = positiveSub(tmp.getValue(),tmp.getPrecision(),\
                                   this->getValue(),this->getPrecision());
                result[0] = '-';
            }
        }
    }
    else {
        //负数加正数
        if (other.isPositive()){
            BignumberImpl tmp = this->getNegete();
            if (other >= tmp)
                result = positiveSub(other.getValue(),other.getPrecision(),tmp.getValue(),tmp.getPrecision());
            else {
                result = positiveSub(tmp.getValue(),tmp.getPrecision(),other.getValue(),other.getPrecision());
                result[0] = '-';
            }
        }
        //负数加负数
        else {
            result = positiveAdd(this->getNegete().getValue(),this->getPrecision(),\
                                 other.getNegete().getValue(),other.getPrecision());
            result[0] = '-';
        }
    }
    return BignumberImpl(result);
}

BignumberImpl BignumberImpl::operator - (const BignumberImpl & other) {
    std::string result;
    if (this->isPositive()){
        //两个正数相减
        if (other.isPositive()) {
            if (*this >= other)
                result = positiveSub(this->getValue(),this->getPrecision(),other.getValue(),other.getPrecision());
            else {
                result = positiveSub(other.getValue(),other.getPrecision(),this->getValue(),this->getPrecision());
                result[0] = '-';
            }
        }
        //正数减负数
        else result = positiveAdd(this->getValue(),this->getPrecision(),\
                                  other.getNegete().getValue(),other.getPrecision());
    }
    else {
        //负数减整数
        if (other.isPositive()) {
            result = positiveAdd(this->getNegete().getValue(),this->getPrecision(),\
                                 other.getValue(),other.getPrecision());
            result[0] = '-';
        }
        //负数减负数
        else return (other.getNegete() - this->getNegete());
    }
    return BignumberImpl(result);
}

std::istream& operator >> (std::istream& in,BignumberImpl& num){
    std::string str;
    in>>str;
    num = BignumberImpl(str);
    std::cout<<"num = "<<num<<std::endl;
    return in;
}

std::ostream& operator << (std::ostream& out,const BignumberImpl& num){
    out<<num.getValue();
    return out;
}

bool BignumberImpl::operator == (const BignumberImpl & other) const {
    return (mark_ == other.mark_) &&\
           (getPrecision() == other.getPrecision()) && \
           (numStr_ == other.numStr_);
}
bool BignumberImpl::operator != (const BignumberImpl & other) const {
    return !(*this == other);
}
bool BignumberImpl::operator < (const BignumberImpl & other) const {
    //根据符号比较大小
    if (this->isNegetive() && other.isPositive()) return true;
    else if (this->isNegetive() && other.isNegetive()) return !(this->getNegete() < other.getNegete());

    //两个正数比较
    else if (this->isPositive() && other.isPositive()) {
        if (this->getIntegerSize() < other.getIntegerSize()) return true;
        else if (this->getIntegerSize() == other.getIntegerSize()) {
            size_t i = 0;
            for (i = 0;i < this->getIntegerSize();++i)
                if (this->numStr_[i] < other.numStr_[i]) return true;
            for (i = 0;i < std::min(this->getPrecision(),other.getPrecision());++i)
                if (this->numStr_[this->getIntegerSize() + i] < other.numStr_[other.getIntegerSize() + i]) return true;
            if (i < other.getPrecision()) return true;
        }
    }

    return false;
}

bool BignumberImpl::operator > (const BignumberImpl & other) const {
    return !(*this < other || *this == other);
}

bool BignumberImpl::operator <= (const BignumberImpl & other) const {
    return (*this < other || *this == other);
}

bool BignumberImpl::operator >= (const BignumberImpl & other) const {
    return (*this > other || *this == other);
}

BignumberImpl BignumberImpl::getNegete() const {
    BignumberImpl negete(*this);
    if (negete.mark_ == Mark::NEGETIVE) negete.mark_ = Mark::POSITIVE;
    else negete.mark_ = Mark::NEGETIVE;
    return negete;
}
BignumberImpl BignumberImpl::getAbs() const {
    BignumberImpl abs(*this);
    abs.mark_ = Mark::POSITIVE;
    return abs;
}
/*************************************************
 * BignumberImpl接受一个字符串构造函数的参数，确保字符串
 * 合法且经过了格式化
 * ***********************************************/
BignumberImpl::BignumberImpl(const std::string & value):dotPos_(0),mark_(Mark::POSITIVE){
    if (value == "") {
        numStr_  =   "0";
    }
    else {
        if (value[0] == '-') mark_ = Mark::NEGETIVE;
        for (size_t i = 1;i < value.size();++i){
            if (isdigit(value[i])) numStr_.push_back(value[i]);
            if (value[i] == '.') dotPos_ = value.size() - i - 1;
        }
    }
}


