#include <stdio.h>

int main()
{
    int bucket_size, output_rate, input_packets[10], n;

    printf("Enter the bucket size: ");
    scanf("%d", &bucket_size);

    printf("Enter the output rate: ");
    scanf("%d", &output_rate);

    printf("Enter the number of time intervals: ");
    scanf("%d", &n);

    printf("Enter the number of packets arriving at each interval:\n");
    for (int i = 0; i < n; i++)
    {
        printf("Interval %d: ", i + 1);
        scanf("%d", &input_packets[i]);
    }

    int bucket_content = 0;

    for (int i = 0; i < n; i++)
    {
        printf("\nTime Interval %d:\n", i + 1);

        if (input_packets[i] + bucket_content > bucket_size)
        {
            printf("Bucket overflow! Dropped packets: %d\n",
                   (input_packets[i] + bucket_content) - bucket_size);
            bucket_content = bucket_size;
        }
        else
        {
            bucket_content += input_packets[i];
            printf("Packets added to bucket. Current bucket size: %d\n",
                   bucket_content);
        }

        if (bucket_content >= output_rate)
        {
            printf("Sent %d packets.\n", output_rate);
            bucket_content -= output_rate;
        }
        else
        {
            printf("Sent %d packets (bucket is now empty).\n", bucket_content);
            bucket_content = 0;
        }

        printf("Remaining in bucket: %d\n", bucket_content);
    }

    while (bucket_content > 0)
    {
        printf("\nEmptying bucket...\n");

        if (bucket_content >= output_rate)
        {
            printf("Sent %d packets.\n", output_rate);
            bucket_content -= output_rate;
        }
        else
        {
            printf("Sent %d packets (bucket is now empty).\n", bucket_content);
            bucket_content = 0;
        }

        printf("Remaining in bucket: %d\n", bucket_content);
    }

    printf("\nSimulation complete.\n");
    return 0;
}
