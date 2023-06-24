////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2023 Mateusz Malinowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <signal.h>
#include <unistd.h>
#include <GenericTalker.h>


std::atomic<bool> run(true);

struct MyDataOne
{
    int mOne;
    double mTwo;
};

struct MyDataTwo
{
    int mOne;
    int mTwo;
};

void signHandler(int)
{
    run = false;
}

class MyListener : public IGenericListener<MyDataOne>, public IGenericListener<MyDataTwo>
{
public:
    virtual void update(const MyDataOne& data) const
    {
        printf("Received data one: %d, %f \n", data.mOne, data.mTwo);
    }
    virtual void update(const MyDataTwo& data) const
    {
        printf("Received data two: %d, %d \n", data.mOne, data.mTwo);
    }
};

class MyTalkerOne : public GenericTalker<MyDataOne>
{
public:
    MyTalkerOne() : GenericTalker<MyDataOne>() {};

    void generateData()
    {
        mMyDataOne.mOne = rand();
        mMyDataOne.mTwo = drand48();

        printf("Broadcasting data one: %d, %f \n", mMyDataOne.mOne, mMyDataOne.mTwo);
        notifyListeners(mMyDataOne);
    }

    virtual int registerListener(const IGenericListener<MyDataOne>& listener)
    {
        puts("Registering listener in talker one");
        return GenericTalker<MyDataOne>::registerListener(listener);
    }
private:
    MyDataOne mMyDataOne;
};

class MyTalkerTwo : public GenericTalker<MyDataTwo>
{
public:
    MyTalkerTwo() : GenericTalker<MyDataTwo>() {};
    
    void generateData()
    {
        mMyDataTwo.mOne = rand();
        mMyDataTwo.mTwo = rand();

        printf("Broadcasting data two: %d, %d \n", mMyDataTwo.mOne, mMyDataTwo.mTwo);
        notifyListeners(mMyDataTwo);
    }

    virtual int registerListener(const IGenericListener<MyDataTwo>& listener)
    {
        puts("Registering listener in talker two");
        return GenericTalker<MyDataTwo>::registerListener(listener);
    }
private:
    MyDataTwo mMyDataTwo;
};

int main()
{
    MyListener listener;
    MyTalkerOne talkerOne;
    MyTalkerTwo talkerTwo;
    int idOne = talkerOne.registerListener(listener);
    int idTwo = talkerTwo.registerListener(listener);
    signal(SIGINT, signHandler);

    while (run)
    {
        talkerOne.generateData();
        talkerTwo.generateData();
        sleep(1);
    }

    talkerOne.unregisterListener(idOne);
    talkerTwo.unregisterListener(idTwo);

    puts("Finished");

    return 0;
}