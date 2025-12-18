#pragma once

#include <array>
#include <cstdint>

template <typename T> class ArrayQueue
{
  private:
    T *const data_;
    size_t readPos_, writePos_, count_;
    const size_t size_;

  public:
    enum class Status
    {
        SUCCESS,
        EMPTY,
        FULL
    };

    template <size_t N>
    ArrayQueue(std::array<T, N> &buf) : ArrayQueue(buf.data(), N)
    {
    }
    ArrayQueue(T *data, size_t size) : data_{data}, readPos_{0}, writePos_{0}, count_{0}, size_{size}
    {
    }
    ~ArrayQueue()
    {
    }

    /**
     * @brief キューに存在する要素の数を返します
     *
     * @return size_t
     */
    size_t count() const
    {
        return count_;
    }

    /**
     * @brief キューが持つことのできる最大要素数を返します
     *
     * @return size_t
     */
    size_t maxSize() const
    {
        return size_;
    }

    Status push(T i);
    Status pop();
    const T &get() const;
};

template <typename T> inline typename ArrayQueue<T>::Status ArrayQueue<T>::push(T i)
{
    if (count_ == size_)
    {
        return Status::FULL;
    }

    data_[writePos_] = i;

    writePos_ += 1;
    writePos_ %= size_; // リングバッファになるように剰余を計算

    count_++;

    return Status::SUCCESS;
}

template <typename T> inline typename ArrayQueue<T>::Status ArrayQueue<T>::pop()
{
    if (count_ == 0)
    {
        return Status::EMPTY;
    }

    T temp;
    temp = data_[readPos_];

    readPos_ += 1;
    readPos_ %= size_;

    count_--;

    return Status::SUCCESS;
}

template <typename T> inline const T &ArrayQueue<T>::get() const
{
    return data_[readPos_];
}
