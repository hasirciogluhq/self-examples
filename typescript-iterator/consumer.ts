import { randomUUIDv7 } from "bun";
import {
  AckPolicy,
  connect,
  DeliverPolicy,
  nanos,
  ReplayPolicy,
  type JetStreamClient,
  type JetStreamManager,
} from "nats";

const ensureConsumerExists = async (
  stream: string,
  durableName: string,
  jsm: JetStreamManager,
) => {
  const consumers = jsm.consumers.list(stream);

  let found = false;
  for await (const consumer of consumers) {
    if (consumer.name === durableName) {
      found = true;
      break;
    }
  }

  if (found) {
    const consumerInfo = await jsm.consumers.update(stream, durableName, {
      max_ack_pending: 1,

      ack_wait: nanos(30 * 1000), // 30 sn içinde ack gelmezse retry

      inactive_threshold: nanos(60 * 1000), // 1 dk offline → auto delete

      max_deliver: 1, // (opsiyonel) retry istemiyorsan

      filter_subject: "payments.>",
    });
  } else {
    await jsm.consumers.add(stream, {
      durable_name: durableName,

      max_ack_pending: 1,

      deliver_policy: DeliverPolicy.Last, // 👈 sadece yeni mesajlar

      ack_policy: AckPolicy.Explicit, // 👈 güvenli processing

      ack_wait: nanos(30 * 1000), // 30 sn içinde ack gelmezse retry

      inactive_threshold: nanos(60 * 1000), // 1 dk offline → auto delete

      max_deliver: 1, // (opsiyonel) retry istemiyorsan

      filter_subject: "payments.>",
    });
  }
};

export const runConsumer = async <T>(
  stream: string,
  durableName: string,
  { emit }: { emit: (data: T) => void },
) => {
  const nc = await connect({});
  const js = nc.jetstream();
  const jsm = await nc.jetstreamManager();

  await ensureConsumerExists(stream, durableName, jsm);

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
    await consume.close();
    await nc.drain();
    await nc.close();
  };
};
