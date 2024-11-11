/* Some of you may wondering how we handle two people requesting the same item in stock at the same time when there is only one ?
well, thanks to that queue system, we handle trade request one by one. => FIFO (first in, first out!)
*/
class ProductQueue
{
    private ref array<ref TraderPlusProduct> queue;

    void ProductQueue()
    {
        queue = new array<ref TraderPlusProduct>;
    }

    int Count()
    {
        return queue.Count();
    }

    bool IsMaxQueue(int max)
    {
      if(queue.Count()>max)
      return true;
      else return false;
    }

    void EnQueue(TraderPlusProduct product)
    {
      queue.Insert(product);
    }

    void DeQueue()
    {
        for(int i = 1; i < queue.Count(); i++)
        {
            queue.Set(i - 1, queue[i]);
        }
        i--;
        queue.RemoveOrdered(i);
    }

    TraderPlusProduct Peek()
    {
        return queue[0];
    }

    bool HasNextQueue()
    {
      return queue.IsValidIndex(0));
    }
}
