/* Some of you may wondering how we handle two people requesting the same item in stock at the same time when there is only one ?
well, thanks to that queue system, we handle trade request one by one. => FIFO (first in, first out!)
*/
class BankingTransactionQueue
{
    private ref array<ref TraderPlusBankingTransaction> queue;

    void BankingTransactionQueue()
    {
        queue = new array<ref TraderPlusBankingTransaction>;
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

    void EnQueue(TraderPlusBankingTransaction product)
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

    TraderPlusBankingTransaction Peek()
    {
        return queue[0];
    }

    bool HasNextQueue()
    {
      return queue.IsValidIndex(0));
    }
}
