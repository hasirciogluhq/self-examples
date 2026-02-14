import { randomUUIDv7 } from "bun";
import { AckPolicy, connect, DeliverPolicy, nanos, ReplayPolicy } from "nats";

export const runConsumer = async <T>(
  stream: string,
  durableName: string,
  { emit }: { emit: (data: T) => void },
) => {
  const nc = await connect({});
  const js = nc.jetstream();
  const jsm = await nc.jetstreamManager();

  const consumerInfo = await jsm.consumers.add(stream, {
    durable_name: durableName,

    deliver_policy: DeliverPolicy.New, // 👈 sadece yeni mesajlar

    ack_policy: AckPolicy.Explicit, // 👈 güvenli processing

    ack_wait: nanos(30 * 1000), // 30 sn içinde ack gelmezse retry

    inactive_threshold: nanos(60 * 1000), // 1 dk offline → auto delete

    max_deliver: 1, // (opsiyonel) retry istemiyorsan

    filter_subject: "payments.>",
  });

  const consumer = await js.consumers.get(stream, durableName);

  const consume = await consumer.consume();

  (async () => {
    for await (const event of consume) {
      emit(JSON.parse(event.data.toString()));
      event.ack();
    }
  })();

  return async () => {
    consume.stop();
  };
};
