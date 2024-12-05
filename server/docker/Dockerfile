# Build stage
FROM node:slim AS base
# Install pnpm
RUN npm install -g pnpm



FROM base AS builder

WORKDIR /app


# Copy package files
COPY package*.json ./
COPY pnpm-lock.yaml ./


# Install dependencies using pnpm
RUN pnpm install --prod

# Copy the rest of the application
COPY . .

# Production stage
FROM base as runner

WORKDIR /app

# Copy built application from builder stage
COPY --from=builder /app ./

# Expose port 8088
EXPOSE 8088

# Start the application using pnpm
CMD ["pnpm", "start"]
