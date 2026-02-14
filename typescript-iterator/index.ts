import { RedisClient } from "bun";
import Elysia, { sse } from "elysia";

export type Context<T> = {
  emit: (value: T) => void;
  cancel: () => void;
};

export type CleanupFn = () => void | Promise<void>;

export type Subscriber<T> = (
  context: Context<T>,
) => void | CleanupFn | Promise<CleanupFn | void>;

export async function* createEventIterator<T>(
  subscriber: Subscriber<T>,
): AsyncGenerator<T> {
  const events: T[] = [];
  let cancelled = false;

  // Create a promise that resolves whenever a new event is added to the events array
  let resolveNext: (() => void) | null = null;

  const emit = (event: T) => {
    events.push(event);
    // If we are awaiting for a new event, resolve the promise
    if (resolveNext) {
      resolveNext();
      resolveNext = null;
    }
  };

  const cancel = () => {
    cancelled = true;
  };

  const unsubscribe = await subscriber({ emit, cancel });

  try {
    while (!cancelled) {
      // If there are events in the queue, yield the next event
      if (events.length > 0) {
        yield events.shift()!;
      } else {
        // Wait for the next event
        await new Promise<void>((resolve) => {
          resolveNext = resolve;
        });
      }
    }

    // Process any remaining events that were emitted before cancellation.
    while (events.length > 0) {
      yield events.shift()!;
    }
  } finally {
    await unsubscribe?.();
  }
}

export async function* createTestEventIterator() {
  const events: string[] = [];
  let cancelled = false;

  // Create a promise that resolves whenever a new event is added to the events array
  let resolveNext: (() => void) | null = null;

  const emit = (event: string) => {
    events.push(event);
    // If we are awaiting for a new event, resolve the promise
    if (resolveNext) {
      resolveNext();
      resolveNext = null;
    }
  };

  const cancel = () => {
    cancelled = true;
    if (resolveNext) {
      resolveNext();
      resolveNext = null;
    }
  };

  let i = 0;
  const id = setInterval(() => {
    if (i > 3) {
      cancel();
    } else {
      emit("hello world: " + i);
      i++;
    }
  }, 1000);

  try {
    while (!cancelled) {
      // If there are events in the queue, yield the next event
      if (events.length > 0) {
        yield events.shift()!;
      } else {
        // Wait for the next event
        await new Promise<void>((resolve) => {
          resolveNext = resolve;
        });
      }
    }

    // Process any remaining events that were emitted before cancellation.
    while (events.length > 0) {
      yield events.shift()!;
    }
  } finally {
    clearInterval(id);
  }
}

function redisEventIterator(channel: string) {
  const client = new RedisClient("redis://default:dev-secret@localhost:6379");

  return createEventIterator<string>(async ({ emit, cancel }) => {
    const messageHandler = (channel: string, message: string) => {
      emit(message);
    };

    // Subscribe to the channel
    client.subscribe(channel, messageHandler);

    console.log("subscribed");

    // Cleanup function to unsubscribe and disconnect
    return async () => {
      client.unsubscribe();
      await client.unsubscribe(channel);
      client.close();
    };
  });
}

function testEventIterator() {
  const client = new RedisClient("redis://default:dev-secret@localhost:6379");

  return createEventIterator<string>(async ({ emit, cancel }) => {
    let i = 0;
    const id = setInterval(() => {
      emit("hello world: " + i);
      i++;
    }, 500);

    // Cleanup function to unsubscribe and disconnect
    return async () => {
      client.unsubscribe();
      clearInterval(id);
      client.close();
    };
  });
}

// Usage

// for await (const message of createTestEventIterator()) {
//   console.log("New message:", message);

//   // You can cancel the event stream if needed
//   if (message === "STOP") {
//     break;
//   }
// }

const app = new Elysia()
  .get("/", async function* () {
    return Bun.file("sse.html");
  })
  .get("/sse", async function* () {
    for await (const message of createTestEventIterator()) {
      yield sse(message);
    }
  });

app.listen(7070, () => {
  console.log("Server is running on port 7070, http://localhost:7070");
});
