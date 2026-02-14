import { randomUUIDv7 } from "bun";
import { connect, DeliverPolicy, ReplayPolicy } from "nats";

export const runPublisher = async () => {
  const nc = await connect({});
  const js = nc.jetstream();
  const jsm = await nc.jetstreamManager();

  await jsm.streams.add({
    name: "PAYMENTS",
    subjects: ["payments.>"],
  });

  const interval = setInterval(() => {
    js.publish(
      "payments.completed",
      JSON.stringify({
        id: randomUUIDv7(),
        status: "PAID",
        amount: 400,
        currency: "USD",
      }),
    );
  }, 100);

  return async () => {
    clearInterval(interval);
  };
};
